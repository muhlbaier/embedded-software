/**
 * laser_tag.c
 *
 *  Created on: Apr 18, 2018
 *      Author: Michael
 */
#include "laser_tag.h"
#include "mqtt.h"
#include "timing.h"
#include "task.h"
#include "laser_comms.h"
#include "json_parser.h"
#include "string.h"
#include "msg.h"
#include "aws_iot_error.h"
#include "aws_iot_mqtt_client.h"

/// @todo preferred teams on register

/// @todo setup unregister message as my Will

#ifndef AWS_SUCCESS
#define AWS_SUCCESS 0
#endif

#define AWS_TOPIC_HEADER "$aws/things/"

// player subscribe topics
static char gottem_topic[40] = "";
static const char start_topic[] = "$aws/things/server/lasertag/start";
static const char end_topic[] = "$aws/things/server/lasertag/end";
static char playerout_topic[40] = "";

// player publish topics
static const char register_topic[] = "$aws/things/server/lasertag/register";
//static const char unregister_topic[] = "$aws/things/server/lasertag/unregister";
static char shoot_topic[32] = "";
static char hit_topic[32] = "";

static char * deviceName;

// MQTT callbacks for subscriptions
static void Gottem(AWS_IoT_Client *client, char *topic, uint16_t topic_len, IoT_Publish_Message_Params *params, void *data);
static void Start(AWS_IoT_Client *client, char *topic, uint16_t topic_len, IoT_Publish_Message_Params *params, void *data);
static void End(AWS_IoT_Client *client, char *topic, uint16_t topic_len, IoT_Publish_Message_Params *params, void *data);
static void PlayerOut(AWS_IoT_Client *client, char *topic, uint16_t topic_len, IoT_Publish_Message_Params *params, void *data);

// IR receiver callback
static void ImHit(uint8_t id);

static laser_tag_status_t status;

static void(*status_callback)(const laser_tag_status_t *);

enum game_modes {LAST_TEAM_STANDING};

typedef struct {
  int shot_damage; // as a percentage
  int lives;
  int respawn; // in seconds
  uint32_t fire_rate; // in milliseconds
  enum game_modes mode;
  int points_per_hit;
  int points_per_kill;
  uint32_t last_shot; // timestamp of last shot
} laser_tag_rules_t;

static laser_tag_rules_t rules;

void LaserTag_Init(void) {
  IoT_Error_t rc = FAILURE;
  char register_payload[32];
  MQTT_Init();
  getIoTDeviceConfig(&deviceName);

  // create topic strings
  snprintf(gottem_topic, sizeof(gottem_topic), "$aws/things/%s/lasertag/gottem", deviceName);
  snprintf(playerout_topic, sizeof(playerout_topic), "$aws/things/%s/lasertag/playerout", deviceName);
  snprintf(shoot_topic, sizeof(shoot_topic), "$aws/things/%s/lasertag/shoot", deviceName);
  snprintf(hit_topic, sizeof(hit_topic), "$aws/things/%s/lasertag/hit", deviceName);

  // subscribe to $aws/things/<dev_name>/gottem
  rc = MQTT_Subscribe(gottem_topic, QOS0, Gottem, NULL);
  if(AWS_SUCCESS != rc) {
    msg_error("Error subscribing : %d\n", rc);
  } else {
    msg_info("Subscribed to topic %s\n", gottem_topic);
  }
  // subscribe to $aws/things/start
  rc = MQTT_Subscribe(start_topic, QOS1, Start, NULL);
  if(AWS_SUCCESS != rc) {
    msg_error("Error subscribing : %d\n", rc);
  } else {
    msg_info("Subscribed to topic %s\n", start_topic);
  }
  // subscribe to $aws/things/end
  rc = MQTT_Subscribe(end_topic, QOS1, End, NULL);
  if(AWS_SUCCESS != rc) {
    msg_error("Error subscribing : %d\n", rc);
  } else {
    msg_info("Subscribed to topic %s\n", end_topic);
  }
  // subscribe to $aws/things/<dev_name>/playerout
  rc = MQTT_Subscribe(playerout_topic, QOS1, PlayerOut, NULL);
  if(AWS_SUCCESS != rc) {
    msg_error("Error subscribing : %d\n", rc);
  } else {
    msg_info("Subscribed to topic %s\n", playerout_topic);
  }

  // initialize status
  rules.last_shot = 0;
  status.status = GAME_NOT_STARTED;
  status.my_score = 0;
  status.their_score = 0;
  status.health = 100;
  status.lives = 3;
  status.player_id = 0;
  rules.shot_damage = 20;
  rules.lives = 3;
  rules.respawn = 30;
  rules.fire_rate = 1000;
  rules.mode = LAST_TEAM_STANDING;
  rules.points_per_hit = 1;
  rules.points_per_kill = 10;
  status_callback = 0;
  // register myself (no support for preferred team yet)
  snprintf(register_payload, sizeof(register_payload), "{\"player_name\": \"%s\"}", deviceName);
  MQTT_Publish(register_topic, QOS1, register_payload, strlen(register_payload)+1);
}

void LaserTag_Fire(void) {
  // enforce fire_rate rule
  if(TimeSince(rules.last_shot) >= rules.fire_rate) {
    // must be alive to shoot
    if(status.status == GAME_ON_ALIVE) {
      rules.last_shot = TimeNow();
      MQTT_Publish(shoot_topic, QOS0, "{\"shots\": 1}", 13);
      msg_info("phew\a");
      LaserComms_Fire();
    }
  }
}

void LaserTag_RegisterStatusCallback(void(*callback)(const laser_tag_status_t *)) {
  status_callback = callback;
}

static void Gottem(AWS_IoT_Client *client, char *topic, uint16_t topic_len, IoT_Publish_Message_Params *params, void *data) {
  char * value;
  uint8_t id;
  value = json_find_key("id", params->payload);
  if(value) {
    id = json_get_int(value);
    msg_info("nice shot, you hit %d", id);
  }else {
    msg_info("gottem received without an id");
  }
  if(status_callback) status_callback(&status);
}

static void Start(AWS_IoT_Client *client, char *topic, uint16_t topic_len, IoT_Publish_Message_Params *params, void *data) {
  char * obj;
  char * list_item;
  char * value;
  char name[16];
  int i;
  // get pointer to rules JSON object
  obj = json_find_key("rules", params->payload);
  // get any rules present in the object
  if(obj) {
      value = json_find_key("shot_damage", obj);
      if(value) rules.shot_damage = json_get_int(value);
      value = json_find_key("lives", obj);
      if(value) rules.lives = json_get_int(value);
      value = json_find_key("respawn", obj);
      if(value) rules.respawn = json_get_int(value);
      value = json_find_key("fire_rate", obj);
      if(value) rules.fire_rate = json_get_int(value);
      value = json_find_key("mode", obj);
      if(value) {
	  /// @todo convert mode name to enum when needed
      }
      value = json_find_key("points_per_hit", obj);
      if(value) rules.points_per_hit = json_get_int(value);
      value = json_find_key("points_per_kill", obj);
      if(value) rules.points_per_kill = json_get_int(value);
  }else {
    msg_info("start missing rules object");
  }
  // get pointer to team 1 object (list of objects)
  obj = json_find_key("team1", params->payload);
  if(obj) {
    i = 1;
    while((list_item = json_find_list_item(i, obj)) != 0) {
      value = json_find_key("name", list_item);
      if(value) {
	json_get_string(value, name);
	msg_info("%s on team 1", name);
	// if the name matches my name then it is me
	if(strncmp(value,deviceName,7) != 0) {
	  // get the id
	  value = json_find_key("id", list_item);
	  if(value) status.player_id = json_get_int(value);
	  status.team_id = 0;
	  msg_info("I am ID %d and on team 1", status.player_id);
	}
      }
      i++;
    }
  }else {
    msg_info("start missing team1 object");
  }
  // get pointer to team 1 object (list of objects)
  obj = json_find_key("team2", params->payload);
  if(obj) {
    i = 1;
    while((list_item = json_find_list_item(i, obj)) != 0) {
      value = json_find_key("name", list_item);
      if(value) {
	json_get_string(value, name);
	msg_info("%s on team 2", name);
	// if the name matches my name then it is me
	if(strncmp(value,deviceName,7) != 0) {
	  // get the id
	  value = json_find_key("id", list_item);
	  if(value) status.player_id = json_get_int(value);
	  status.team_id = 1;
	  msg_info("I am ID %d and on team 2", status.player_id);
	}
      }
      i++;
    }
  }else {
    msg_info("start missing team2 object");
  }
  status.lives = rules.lives;
  status.health = 100;
  status.my_score = 0;
  status.their_score = 0;
  // initialize the IR transmitter and receiver
  LaserComms_Init(status.player_id, ImHit);
  // change status to on and alive
  status.status = GAME_ON_ALIVE;
  if(status_callback) status_callback(&status);
}

static void End(AWS_IoT_Client *client, char *topic, uint16_t topic_len, IoT_Publish_Message_Params *params, void *data) {
  msg_info("Game over, json:");
  msg_info(params->payload);
  status.status = GAME_OVER;
  if(status_callback) status_callback(&status);
}

static void PlayerOut(AWS_IoT_Client *client, char *topic, uint16_t topic_len, IoT_Publish_Message_Params *params, void *data) {
  char * value;
  uint8_t id;
  value = json_find_key("id", params->payload);
  if(value) {
    id = json_get_int(value);
    if(id == status.player_id) {
      if(status.lives != 0) {
	msg_warning("I'm not dead yet! (but received player out so I will do the honorable thing and die)");
	status.status = GAME_ON_DEAD;
	status.lives = 0;
	status.health = 0;
      }else {
	msg_warning("goodbye cruel laser world");
	status.status = GAME_ON_DEAD;
	status.health = 0;
      }
    }else {
	msg_info("Player %d bit the dust", id);
    }
    if(status_callback) status_callback(&status);
  }
}

static void RespawnMe(void) {
  if(status.status == GAME_ON_RESPAWN) {
      status.status = GAME_ON_ALIVE;
      if(status_callback) status_callback(&status);
      msg_info("I'm Baaaaaaaaack!");
  }
}

static void ImHit(uint8_t id) {
  char hit_payload[16];
  snprintf(hit_payload, sizeof(hit_payload), "{\"id\": %d}", id);
  MQTT_Publish(hit_topic, QOS1, hit_payload, strlen(hit_payload)+1);
  if(status.health > rules.shot_damage) status.health -= rules.shot_damage;
  else {
      if(status.lives) status.lives--;
      if(status.lives) {
	  status.status = GAME_ON_RESPAWN;
	  status.health = 100;
	  Task_Schedule(RespawnMe, 0, rules.respawn*1000, 0);
      }else {
	  status.status = GAME_ON_DEAD;
	  status.health = 0;
      }
  }
  if(status_callback) status_callback(&status);
  msg_info("ouch, player ID %d shot me", id);
}

void LaserComms_Init(uint8_t playerid, void (* hit)(uint8_t)) {

}

void LaserComms_Fire(void) {

}

void LaserComms_HitFilter(uint32_t limit) {

}
