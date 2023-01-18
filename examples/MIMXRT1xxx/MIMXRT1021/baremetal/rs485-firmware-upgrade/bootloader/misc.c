#include "misc.h"

void getDNumMISC(uint8_t *dnum, uint32_t num)
{ /// calculate decimal digits from integer number 'num' and store them as ASCII char in dnum array
    uint8_t k;
    uint32_t step = 1000000000;

    for (k = 0; k < 10; k++)
    {                     // convert integer value in hex format to decimal format
        dnum[9 - k] = 48; // set char value
        while (num >= step)
        {
            dnum[9 - k]++;
            num -= step;
        }
        step /= 10;
    }
}

void putDNumMISC(uint8_t *dnum, uint16_t *m, uint8_t *r_str)
{ /// add unsigned integer number digits into string r_str
    uint8_t k = 0, flag = 0;

    for (k = 0; k < 10; k++)
    { // convert integer value in hex format to decimal format
        if (flag == 0)
        {
            if ((dnum[9 - k] != 48) || (k == 9)) // include the 0x00000000 number
            {
                flag = 1;
                r_str[(*m)] = dnum[9 - k];
                (*m)++;
            }
        }
        else
        {
            r_str[(*m)] = dnum[9 - k];
            (*m)++;
        }
    }
     r_str[(*m)] = 0x00;
}

void getStr4NumMISC(uint16_t type, uint32_t  * num, uint8_t * rstr)
{   /// print text and one signed integer or float number
    // the 'num' number is not modified!
    int32_t * p_int32;
    uint8_t dnum[11];
    uint8_t k;
    uint16_t m = 0;
    rstr[0] = 0x00;
    
    switch(type)
    {
		case(PRINT_ARG_TYPE_BINARY_BYTE):
		{// 8 bit binary representation
			for(k=0;k<8;k++)
			{
				if((*num) & 0x00000080)
				{
					rstr[k] = '1';
				}
				else
				{
					rstr[k] = '0';
				}
				*num = (*num)<<1;
			}
			rstr[k] = 0x00;
			break;
		}
		case(PRINT_ARG_TYPE_BINARY_HALFWORD):
		{// 16 bit binary representation
			for(k=0;k<16;k++)
			{
				if((*num) & 0x00008000)
				{
					rstr[k] = '1';
				}
				else
				{
					rstr[k] = '0';
				}
				*num = (*num)<<1;
			}
			rstr[k] = 0x00;
			break;
		}
		case(PRINT_ARG_TYPE_BINARY_WORD):
		{// 32 bit binary representation
			for(k=0;k<32;k++)
			{
				if((*num) & 0x80000000)
				{
					rstr[k] = '1';
				}
				else
				{
					rstr[k] = '0';
				}
				*num = (*num)<<1;
			}
			rstr[k] = 0x00;
			break;
		}
		case(PRINT_ARG_TYPE_DECIMAL_BYTE):
		case(PRINT_ARG_TYPE_DECIMAL_HALFWORD):
		case(PRINT_ARG_TYPE_DECIMAL_WORD):
		{// conver integer number into appropriate string
			p_int32 = (int32_t *)num;
            if (*p_int32 < 0)
            { // print the sign character
                rstr[m] = '-';
                m++;
                *p_int32 = (*p_int32)*(-1);
            }

            getDNumMISC(dnum, (uint32_t)(*p_int32));
            putDNumMISC(dnum, &m, rstr);
			break;
		}
		case(PRINT_ARG_TYPE_CHARACTER):
		{
			break;
		}
		case(PRINT_ARG_TYPE_STRING):
		{
			break;
		}
		case(PRINT_ARG_TYPE_FLOAT):
		{
			//int32_t exp;
			//uint32_t man;
			//// extract sign
			//if((*num)&0x80000000)
			//{
				//rstr[m] = '-';
				//m++;
			//}
			
			//// extract exponent
			//exp = (((*num)&0x7F800000)>>23);
			//exp -= 127;
			
			//// extract mantisa
			//man = ~((*num)&0xFF800000);
			
			////exp = (*num);
			//getDNumMISC(dnum, exp);
            //putDNumMISC(dnum, &m, rstr);
			
			
			float f_step;
			float f_num = *((float *)num); // convert pointer
			uint8_t sige = ' ';
			uint16_t e_part;
			uint32_t d_part, i_part;
			
            if (f_num < 0)
            { // negative float number
                rstr[m] = '-';
                m++;
                f_num = (-1) * f_num;
            }

            if (f_num < 1)
            {
                sige = '-';
                f_step = 0.1;
                e_part = 1;
                while (f_num < f_step)
                {
                    f_step = f_step * 0.1;
                    e_part++;
                }
            }
            else
            {
                f_step = 1.0;
                e_part = 0;
                while (f_num > f_step)
                {
                    f_step = f_step * 10;
                    e_part++;
                }
                f_step = f_step / 10;
                e_part--;
            }

            f_num = f_num / f_step;
            i_part = (uint32_t)f_num;

            f_num = f_num - (float)i_part;
            d_part = (uint32_t)(1000 * f_num);

            getDNumMISC(dnum, i_part);
            putDNumMISC(dnum, &m, rstr);

            getDNumMISC(dnum, d_part);
            dnum[3] = '.';
            putDNumMISC(dnum, &m, rstr);

            rstr[m] = 'e';
            m++;

            if (sige == '-')
            {
                rstr[m] = '-';
                m++;
            }

            getDNumMISC(dnum, e_part);
            putDNumMISC(dnum, &m, rstr);
				
			break;
		}
		case(PRINT_ARG_TYPE_HEXADECIMAL_BYTE):
		{// 8 bit hex string representation
			uint8_t t_rez;
            for (k=0;k<2;k++)
            {
                t_rez = ((*num) & 0x000000F0) >> 4;
                if (t_rez < 0x0A)
                {
                    rstr[m] = t_rez + 0x30;
                }
                else
                {
                    rstr[m] = t_rez + 0x41 - 0x0A;
                }
                (*num) = (*num) << 4;
                m++;
            }
            rstr[m] = 0x00;
			break;
		}
		case(PRINT_ARG_TYPE_HEXADECIMAL_HALFWORD):
		{// 16 bit hex string representation
			uint8_t t_rez;
            for (k=0;k<4;k++)
            {
                t_rez = ((*num) & 0x0000F000) >> 12;
                if (t_rez < 0x0A)
                {
                    rstr[m] = t_rez + 0x30;
                }
                else
                {
                    rstr[m] = t_rez + 0x41 - 0x0A;
                }
                (*num) = (*num) << 4;
                m++;
            }
            rstr[m] = 0x00;
			break;
		}
		case(PRINT_ARG_TYPE_HEXADECIMAL_WORD):
		{// 32 bit hex string representation
            uint8_t t_rez;
            for (k=0;k<8;k++)
            {
                t_rez = ((*num) & 0xF0000000) >> 28;
                if (t_rez < 0x0A)
                {
                    rstr[m] = t_rez + 0x30;
                }
                else
                {
                    rstr[m] = t_rez + 0x41 - 0x0A;
                }
                (*num) = (*num) << 4;
                m++;
            }
            rstr[m] = 0x00;
			break;
		}
		default:
		{
			rstr[0] = 0x00;
			break;
		}
	}
}

void getStrMISC(uint8_t * str, uint32_t  * num, uint8_t * r_str)
{/// print text and one signed integer or float number
  // the 'num' number is not modified!
	uint8_t dnum[10],flag = 0;
	uint16_t e_part;
	uint16_t n = 0, m = 0, k = 0;
	int32_t * i_ptr;
	int32_t i_num;
	uint32_t d_part, i_part;
	uint8_t sige = ' ';
	float * f_ptr;
	float f_num;
	float f_step;

	while(str[n] != '\0')
	{
		if(flag == 0)
		{// search for '%' character
			if(str[n] == '%')
			{// found it!
				flag = 1;
			}
			else
			{// this is not it! print the character
				r_str[m] = str[n];
				m++;
			}
			n++;
			
		}
		else if(flag == 1)
		{// test for number type  
			if(str[n] == 'd') 
			{// the given number is signed integer 32bit
				flag = 2;
			}
			if(str[n] == 'c')
			{
				flag = 3;
			}
			else if(str[n] == 'f')
			{// the given number is signed float 32bit
				flag = 4;
			}
			else if(str[n] == 's')
			{//  need to implement
				flag = 5;
			}
			else if(str[n] == 'b')
			{//  need to implement
				flag = 6;
			}
			else if(str[n] == 'h')
			{//  need to implement
				flag = 7;
			}
			else if(str[n] == 'H')
			{
				flag = 8;
			}
			else if(str[n] == 'x')
			{
				flag = 9;
			}
			else
			{// other conversions
			}
		}
		else if(flag == 2)
		{// add the signed integer number
			i_ptr = (int32_t *)num;
			i_num = *i_ptr;
			if(i_num < 0)	
			{											// print the sign character
				r_str[m] = '-';
				m++;
				i_num = i_num*(-1);
			}

			getDNumMISC(dnum, (uint32_t)i_num);
			putDNumMISC(dnum, &m, r_str);
			
			n++;
			flag = 10;
		}
		else if(flag == 3)
		{// add single character		
			r_str[m] = (char)(*num);
			m++;
			n++;
			flag = 10;
		}
		else if(flag == 4)
		{// add the float number
			f_ptr = (float *)num;										// convert pointer
			f_num = *f_ptr;												// copy value
			
			if(f_num < 0)
			{// negative float number
				r_str[m] = '-';
				m++;
				f_num = (-1)*f_num;
			}
			
			if(f_num < 1)
			{
				sige = '-';
				f_step = 0.1;
				e_part = 1;
				while(f_num < f_step)
				{
					f_step = f_step*0.1;
					e_part++;
				}	
			}
			else
			{
				f_step = 1.0;
				e_part = 0;
				while(f_num > f_step)
				{
					f_step = f_step*10;
					e_part++;
				}
				f_step = f_step/10;
				e_part--;			
			}
			
			f_num = f_num/f_step;
			i_part = (uint32_t)f_num;
			
			f_num = f_num - (float)i_part;
			d_part = (uint32_t)(1000*f_num);
			
			getDNumMISC(dnum, i_part);
			putDNumMISC(dnum, &m, r_str);

			getDNumMISC(dnum, d_part);
			dnum[3] = '.';
			putDNumMISC(dnum, &m, r_str);
			
			r_str[m] = 'e';
			m++;
			
			if(sige == '-')
			{
				r_str[m] = '-';
				m++;
			}
				
				
			getDNumMISC(dnum, e_part);
			putDNumMISC(dnum, &m, r_str);
			
			n++;
			flag = 10;
		}
		else if(flag == 5)
		{// add the string 
			uint8_t * t_num = (uint8_t *)num;
			while(t_num[k] != '\0')
			{
				r_str[m] = t_num[k];
				m++;
				k++;
			}
			n++;
			flag = 10;
		}		
		else if(flag == 6)
		{// add the binary data 
			uint32_t b_num = *num;
			uint8_t g = 0;
			for(g=0;g<32;g++)
			{
				if(b_num&0x80000000)
				{
					r_str[m] = '1';
				} 
				else
				{
					r_str[m] = '0';					
				}
				b_num = b_num<<1;
				m++;
			}
			n++;
			flag = 10;
		}		
		else if(flag == 7)
		{// add the hex data 
			uint32_t b_num = *num;
			uint8_t t_rez, g;
			
			r_str[m] = '0';
			r_str[m+1] = 'x';
			m+=2;
			
			for(g=0;g<8;g++)
			{
				t_rez = (b_num&0xF0000000)>>28;
				if(t_rez < 0x0A)
				{
					r_str[m] = t_rez + 0x30;
				} 
				else
				{
					r_str[m] = t_rez + 0x41 - 0x0A;					
				}
				b_num = b_num<<4;
				m++;
			}
			n++;
			flag = 10;
		}
		else if(flag == 8)
		{
			uint8_t b_num = (uint8_t)*num;
			uint8_t tmp_num;
			
			r_str[m] = '0';
			r_str[m+1] = 'x';
			m+=2;
			
			tmp_num = (b_num>>4)&0x0F;
			if(tmp_num < 0x0A)
			{
				r_str[m] = tmp_num + 0x30;
			}
			else
			{
				r_str[m] = tmp_num + 0x41 - 0x0A;
			}
			
			m++;	
			tmp_num = (0x0F & b_num);
			if(tmp_num < 0x0A)
			{
				r_str[m] = tmp_num + 0x30;
			}
			else
			{
				r_str[m] = tmp_num + 0x41 - 0x0A;
			}
			m++;	
			n++;
			flag = 10;	
		}
		else if(flag == 9)
		{// add the float number in form 0.0000
			uint8_t b_num = (uint8_t)*num;
			uint8_t tmp_num;
			
			tmp_num = (b_num>>4)&0x0F;
			if(tmp_num < 0x0A)
			{
				r_str[m] = tmp_num + 0x30;
			}
			else
			{
				r_str[m] = tmp_num + 0x41 - 0x0A;
			}
			
			m++;	
			tmp_num = (0x0F & b_num);
			if(tmp_num < 0x0A)
			{
				r_str[m] = tmp_num + 0x30;
			}
			else
			{
				r_str[m] = tmp_num + 0x41 - 0x0A;
			}
			m++;	
			n++;
			flag = 10;		
		}
		else
		{// add remaining characters
			r_str[m] = str[n];
			n++;
			m++;
		}
	}
	r_str[m] = '\0';
}

void cpyStrMISC(uint8_t * dst, uint8_t * src)
{
	uint16_t k = 0;
	
	while(src[k] != '\0')
	{
		dst[k] = src[k];
		k++;
	}
	dst[k] = '\0';
}

void catStrMISC(uint8_t * dst, uint8_t * src)
{
	uint16_t k = 0;
	uint16_t n = 0;
	
	while(dst[k] != '\0')
	{
		k++;
	}
	
	while(src[n] != '\0')
	{
		dst[k] = src[n];
		n++;
		k++;
	}
	dst[k] = '\0';
}

uint16_t lenStrMISC(uint8_t * str)
{
	uint16_t k = 0;
	
	while(str[k] != '\0')
	{
		k++;
	}
	
	return k;
}

uint8_t cmpStrMISC(uint8_t * spat, uint8_t * stest)
{
	uint16_t len_spat = lenStrMISC(spat);
	uint16_t len_stest = lenStrMISC(stest);
	uint16_t k;
	
	if(len_spat != len_stest)
		return (MISC_STR_LEN_DIFFERS);
		
	for(k=0;k<len_spat;k++)
	{
		if(spat[k] != stest[k])
			return (MISC_STR_DO_NOT_MATCH);
	}
	
	return (MISC_STR_MACTH);
}
		
uint8_t cmpStrWithLenMISC(uint8_t * spat, uint16_t len_spat, uint8_t * stest, uint16_t len_stest)
{
	uint16_t k;
	
	if(len_spat != len_stest)
		return (MISC_STR_LEN_DIFFERS);
		
	for(k=0;k<len_spat;k++)
	{
		if(spat[k] != stest[k])
			return (MISC_STR_DO_NOT_MATCH);
	}
	
	return (MISC_STR_MACTH);
}

void getStr4AddrMISC(uint8_t * str, uint32_t addr)
{/// convert 32bit IP address to human readable string 
	uint8_t num, tmp, k, n=0;
	uint8_t sv[4] = {24,16,8,0};
	uint32_t mask = 0xFF000000;
	uint8_t flag ;
	for(k=0;k<4;k++)
	{
		num = (addr&mask)>>sv[k];
		mask = mask>>8;
		flag = 0;
		
		tmp = num/100;
		if(tmp > 0)
		{
			str[n] = tmp + 48;
			num = num - tmp*100;
			n++;
			flag = 1;
		}
		
		tmp = num/10;
		if(tmp > 0)
		{
			str[n] = tmp + 48;
			num = num - tmp*10;
			n++;
		}
		else
		{
			if(flag == 1)
			{
				str[n] = 48;
				n++;
			}
		}
		
		str[n] = num + 48;
		n++;
		
		if(k != 3)
		{
			str[n] = '.';
			n++;
		}
		else
		{
			str[n] = '\0';
		}
	}
}
