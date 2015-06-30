/******************************************************************************/
/* Change log                                                                 *
 *
 *
 *
 * Date         Revision    Comments
 * MM/DD/YY
 * --------     ---------   ----------------------------------------------------
 * 04/02/15     1.0_DW0a    Initial project make.
 *                          Derived from project 'PIC_PS2_to_UART'.
 *                          Fixed bugs.
 *                          Added features.
 *                          Add new function to check for alphanumeric and
 *                            number.
 *                          Changed start-up sound to say "sitcom generator".
/******************************************************************************/

/******************************************************************************/
/* Contains Miscellaneous Functions
 *
/******************************************************************************/

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#include <xc.h>         /* XC8 General Include File */

#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>        /* For true/false definition */
#include <math.h>        /* For true/false definition */

#include "MISC.h"
#include "system.h"
#include "user.h"

/******************************************************************************/
/* Functions                                                                  */
/******************************************************************************/

/******************************************************************************/
/* MSC_DelayUS
 * Input the number in microseconds to be delayed.
 *
 * The function waists loops for the entered bumber of cycles.
 * The actual delay can be scewed when interrupts are enabled.
/******************************************************************************/
void MSC_DelayUS(long US)
{
    long i;

    for(i=0; i<US; i++)
    {

        Nop();
        Nop();
        Nop();
        Nop();
        Nop();
        Nop();
        Nop();
        Nop();
        Nop();
        Nop();
        Nop();
        Nop();
        Nop();
        Nop();
        Nop();
        Nop();
        Nop();
        Nop();
        Nop();
    }
}

/******************************************************************************/
/* MSC_DelayNOP
 *
 * The function waists the number of cycles passed into the function.
/******************************************************************************/
void MSC_DelayNOP(unsigned char NOPs)
{
    unsigned char i;
    for(i=0; i<NOPs; i++)
    {
        Nop();
    }
}

/******************************************************************************/
/* IsLetter
 *
 * The function returns true if the data is a Letter.
/******************************************************************************/
unsigned char IsLetter(unsigned char data)
{
    if((data >= 65 && data <= 90) || (data >= 97 && data <= 122))
    {
        return TRUE;
    }
    return FALSE;
}

/******************************************************************************/
/* MSC_IsNumber
 *
 * The function returns true if the data is a number.
/******************************************************************************/
unsigned char MSC_IsNumber(unsigned char data)
{
    if(data >= '0' && data <= '9')
    {
        return TRUE;
    }
    return FALSE;
}

/******************************************************************************/
/* MSC_IsCharacter
 *
 * The function returns true if the data is a printable character.
/******************************************************************************/
unsigned char MSC_IsCharacter(unsigned char data)
{
    if(data >= 32 && data <= 126)
    {
        return TRUE;
    }
    return FALSE;
}

/******************************************************************************/
/* MSC_IsAlphaNumeric
 *
 * The function returns true if the data is either a letter or a number or
 *  a space.
/******************************************************************************/
unsigned char MSC_IsAlphaNumeric(unsigned char data)
{
    if(IsLetter(data) || MSC_IsNumber(data) || data == ' ')
    {
        return TRUE;
    }
    return FALSE;
}

/******************************************************************************/
/* MSC_IsAlphaNumericString
 *
 * The function returns true if the string contains only letters and numbers
 *  and spaces.
/******************************************************************************/
unsigned char MSC_IsAlphaNumericString(unsigned char* data)
{
    while(*data != 0)
    {
        if(!MSC_IsAlphaNumeric(*data))
        {
            return FALSE;
        }
        data++;
    }
    return TRUE;
}

/******************************************************************************/
/* MSC_CleanBuffer
 *
 * This function sets an amount of data in the array as 0.
/******************************************************************************/
void MSC_CleanBuffer(unsigned char* data, unsigned int count)
{
    unsigned int i=0;
    for(i=0; i<count;i++)
    {
        data[i]= 0;
    }
}

/******************************************************************************/
/* MSC_CleanBufferInt
 *
 * This function sets an amount of data in the array as 0.
/******************************************************************************/
void MSC_CleanBufferInt(unsigned int* data, unsigned int count)
{
    unsigned int i=0;
    for(i=0; i<count;i++)
    {
        data[i]= 0;
    }
}

/******************************************************************************/
/* MSC_Round
 *
 * This function rounds to the nearest whole number.
/******************************************************************************/
double MSC_Round(double input)
{
    long temp = (long)(input + 0.5);

    return temp;
}

/******************************************************************************/
/* MSC_BufferCopy
 *
 * This function copies the 'from' array to the 'to' array.
/******************************************************************************/
void MSC_BufferCopy(unsigned char* from,unsigned char* to, unsigned int count, unsigned int shift)
{
    unsigned int i=0;
    MSC_CleanBuffer(to,count);
    for(i = shift; i>0; i--)
    {
        *to = ' ';
        to++;
    }
    while(*from != 0 && count >0)
    {
        *to = *from;
        from++;
        to++;
        count--;
    }
}

/******************************************************************************/
/* MSC_StringMatch
 *
 * This function returns TRUE if the array 'This' matches the array 'That'.
/******************************************************************************/
unsigned char MSC_StringMatch(const unsigned char* This, const unsigned char* That)
{
    while(*This != 0)
    {
       if(*This != *That || *That == 0)
       {
           return FALSE;
       }
       This++;
       That++;
    }
    if(*That == 0)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/******************************************************************************/
/* MSC_StringMatchCaseInsensitive
 *
 * This function returns TRUE if the array 'This' matches the array 'That' and
 *  it is not case sensitive.
/******************************************************************************/
unsigned char MSC_StringMatchCaseInsensitive(const unsigned char* This, const unsigned char* That)
{
    unsigned char   tempThis,
                    tempThat;

    while(*This != 0)
    {
        tempThis = *This;
        tempThat = *That;
        MSC_LowercaseChar(&tempThis);
        MSC_LowercaseChar(&tempThat);

       if((tempThis != tempThat) || (tempThat == 0))
       {
           return FALSE;
       }
       This++;
       That++;
    }
    if(*That == 0)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/******************************************************************************/
/* MSC_StringContains
 *
 * This function returns TRUE if the array 'That' is contained in the array
 *   'This'.
/******************************************************************************/
unsigned char MSC_StringContains(const unsigned char* This, const unsigned char* That)
{
    unsigned char   MatchCount = 0;
    unsigned char   MatchAmount = 0;
    unsigned char i = 0;

    for(i=0;i<254;i++)
    {
        if(That[i] == 0)
        {
            MatchAmount = i;
            break;
        }
    }
    while(*This != 0)
    {
        if(*This == *That)
        {
           MatchCount++;
           That++;
           if(*That == 0)
           {
                if(MatchAmount == MatchCount)
                {
                    return TRUE;
                }
                return FALSE;
           }
           if(*This == 0)
           {
               return FALSE;
           }
        }
        else
        {
            That -= MatchCount;
            MatchCount = 0;
        }
        This++;
    }
    return FALSE;
}

/******************************************************************************/
/* MSC_StringContainsCaseInsensitive
 *
 * This function returns TRUE if the array 'That' is contained in the array
 *   'This' and it is not case sensitive.
/******************************************************************************/
unsigned char MSC_StringContainsCaseInsensitive(const unsigned char* This,const unsigned char* That)
{
    unsigned char   tempThis,
                    tempThat;
    unsigned char   MatchCount = 0;
    unsigned char   MatchAmount = 0;
    unsigned char i = 0;

    for(i=0;i<254;i++)
    {
        if(That[i] == 0)
        {
            MatchAmount = i;
            break;
        }
    }
    while(*This != 0)
    {
        tempThis = *This;
        tempThat = *That;
        MSC_LowercaseChar(&tempThis);
        MSC_LowercaseChar(&tempThat);

        if(tempThis == tempThat)
        {
           MatchCount++;
           That++;
           if(*That == 0)
           {
                if(MatchAmount == MatchCount)
                {
                    return TRUE;
                }
                return FALSE;
           }
           if(*This == 0)
           {
               return FALSE;
           }
        }
        else
        {
            That -= MatchCount;
            MatchCount = 0;
        }
        This++;
    }
    return FALSE;
}

/******************************************************************************/
/* MSC_LowercaseString
 *
 * This function turns all characters to in the string to lowercase.
/******************************************************************************/
void MSC_LowercaseString(unsigned char* Input)
{
    while((*Input != 0))
    {
        if((*Input >= 65) && (*Input <= 90 ))
        {
            *Input += 32;
        }
        Input++;
    }
}

/******************************************************************************/
/* MSC_LowercaseChar
 *
 * This function turns the character to lowercase.
/******************************************************************************/
void MSC_LowercaseChar(unsigned char* Input)
{
    if((*Input >= 65) && (*Input <= 90 ))
    {
        *Input += 32;
    }
}

/******************************************************************************/
/* MSC_ReverseByte
 *
 * The function reads the value of 'This' and returns the reverse of the data.
/******************************************************************************/
unsigned char MSC_ReverseByte(unsigned char This)
{
    unsigned char temp=0;

    temp += (This & 0x01) << 7;
    temp += (This & 0x02) << 5;
    temp += (This & 0x04) << 3;
    temp += (This & 0x08) << 1;
    temp += (This & 0x10) >> 1;
    temp += (This & 0x20) >> 3;
    temp += (This & 0x40) >> 5;
    temp += (This & 0x80) >> 7;

    return temp;
}

/******************************************************************************/
/* MSC_ReverseInt
 *
 * The function reads the value of 'This' and returns the reverse of the data.
/******************************************************************************/
unsigned int MSC_ReverseInt(unsigned int This)
{
    unsigned int temp=0;

    temp += (This & 0x0001) << 15;
    temp += (This & 0x0002) << 13;
    temp += (This & 0x0004) << 11;
    temp += (This & 0x0008) << 9;
    temp += (This & 0x0010) << 7;
    temp += (This & 0x0020) << 5;
    temp += (This & 0x0040) << 3;
    temp += (This & 0x0080) << 1;
    temp += (This & 0x0100) >> 1;
    temp += (This & 0x0200) >> 3;
    temp += (This & 0x0400) >> 5;
    temp += (This & 0x0800) >> 7;
    temp += (This & 0x1000) >> 9;
    temp += (This & 0x2000) >> 11;
    temp += (This & 0x4000) >> 13;
    temp += (This & 0x8000) >> 15;

    return temp;
}

/******************************************************************************/
/* MSC_ReverseLong
 *
 * The function reads the value of 'This' and returns the reverse of the data.
/******************************************************************************/
unsigned long MSC_ReverseLong(unsigned long This)
{
    unsigned long temp=0;

    temp += (This & 0x00000001) << 31;
    temp += (This & 0x00000002) << 29;
    temp += (This & 0x00000004) << 27;
    temp += (This & 0x00000008) << 25;
    temp += (This & 0x00000010) << 23;
    temp += (This & 0x00000020) << 21;
    temp += (This & 0x00000040) << 19;
    temp += (This & 0x00000080) << 17;
    temp += (This & 0x00000100) << 15;
    temp += (This & 0x00000200) << 13;
    temp += (This & 0x00000400) << 11;
    temp += (This & 0x00000800) << 9;
    temp += (This & 0x00001000) << 7;
    temp += (This & 0x00002000) << 5;
    temp += (This & 0x00004000) << 3;
    temp += (This & 0x00008000) << 1;
    temp += (This & 0x00010000) >> 1;
    temp += (This & 0x00020000) >> 3;
    temp += (This & 0x00040000) >> 5;
    temp += (This & 0x00080000) >> 7;
    temp += (This & 0x00100000) >> 9;
    temp += (This & 0x00200000) >> 11;
    temp += (This & 0x00400000) >> 13;
    temp += (This & 0x00800000) >> 15;
    temp += (This & 0x01000000) >> 17;
    temp += (This & 0x02000000) >> 19;
    temp += (This & 0x04000000) >> 21;
    temp += (This & 0x08000000) >> 23;
    temp += (This & 0x10000000) >> 25;
    temp += (This & 0x20000000) >> 27;
    temp += (This & 0x40000000) >> 29;
    temp += (This & 0x80000000) >> 31;

    return temp;
}

/******************************************************************************/
/* MSC_GetNumber
 *
 * This function resembles scanf. Use 0x...for hex number.
/******************************************************************************/
unsigned char MSC_GetNumber(unsigned char* This, unsigned char CommaNumber, long* result)
{
    unsigned char i =0;
    long temp =0;
    unsigned char negative =0;
    unsigned char tempCommaNumber = 1;
    unsigned char count =0;
    unsigned char Hex = FALSE;
    unsigned char tempThis;

    while(*This != 0)
    {
        if(*This == '0')
        {
            count++;
            This++;
            if(*This == 'x' || *This == 'X')
            {
                This++;
                if(*This >='0' && *This <='9')
                {
                    Hex = TRUE;
                }
            }
        }
        count++;
        This++;
    }
    This-=count;

    SCAN:while(*This != '=' && *This != ',' && *This != 0)
    {
        /* go all the way to the first comma */
        This++;
    }
    if(*This == 0)
    {
        return NOCOMMA; // there is no equal
    }
    else if(*This != '=' || *This != ',')
    {
        if(tempCommaNumber < CommaNumber)
        {
            tempCommaNumber++;
            This++;
            goto SCAN;
        }
    }
    if(*This == 0)
    {
        return NOCOMMA; // there is no equal
    }
    This++;
    if(*This == ' ')
    {
      This++;
      //move past the space
    }
    else if (*This == '=' || *This == ',')
    {
         return DOUBLECOMMA; //double equal
    }
    else if(*This == '-')
    {
      negative = 1;
      This++;
      //move past the space
    }
    else if(*This == 0)
    {
        return NOVALUE; // there is no value after the equal
    }
    if(Hex)
    {
        negative = FALSE;
        while(*This != 'x' && *This != 'X')
        {
            /* go to first number after the x */
            This++;
        }
        This++;
    }
    while(*This != 0 && *This != ' ' && *This != ',' && *This != '=')
    {
        if(!Hex)
        {
            if(*This >=48 && *This <= 57)
            {
                temp = temp * 10;
                temp += *This - 48;
                i++;
                if(i>32)
                {
                    return TOOBIG;//number too big
                }
            }
        }
        else
        {
            if((*This >=48 && *This <= 57) || (*This >=65 && *This <= 70) || (*This >= 97 && *This <= 102))
            {
                temp <<= 4;
                tempThis = *This;
                if(IsLetter(tempThis))
                {
                    MSC_LowercaseChar(&tempThis);
                    temp += (tempThis - 87);
                }
                else
                {
                    temp += (*This - 48);
                }
                i++;
                if(i>32)
                {
                    return TOOBIG;//number too big
                }
            }
        }
        This++;
    }
    if(negative)
    {
        temp = (temp * -1);
    }
    *result = temp;
    return NoError;
}

/******************************************************************************/
/* MSC_GetNumberUnsigned
 *
 * This function resembles scanf. Use 0x...for hex number.
/******************************************************************************/
unsigned char MSC_GetNumberUnsigned(unsigned char* This, unsigned char CommaNumber, unsigned long* result)
{
    unsigned char i =0;
    long temp =0;
    unsigned char tempCommaNumber = 1;
    unsigned char count =0;
    unsigned char Hex = FALSE;
    unsigned char tempThis;

    while(*This != 0)
    {
        if(*This == '0')
        {
            count++;
            This++;
            if(*This == 'x' || *This == 'X')
            {
                This++;
                if(*This >='0' && *This <='9')
                {
                    Hex = TRUE;
                }
            }
        }
        count++;
        This++;
    }
    This-=count;

    SCAN:while(*This != '=' && *This != ',' && *This != 0)
    {
        /* go all the way to the first comma */
        This++;
    }
    if(*This == 0)
    {
        return NOCOMMA; // there is no equal
    }
    else if(*This != '=' || *This != ',')
    {
        if(tempCommaNumber < CommaNumber)
        {
            tempCommaNumber++;
            This++;
            goto SCAN;
        }
    }
    if(*This == 0)
    {
        return NOCOMMA; // there is no equal
    }
    This++;
    if(*This == ' ')
    {
      This++;
      //move past the space
    }
    else if (*This == '=' || *This == ',')
    {
         return DOUBLECOMMA; //double equal
    }
    else if(*This == '-')
    {
        return NEGATIVE; // this is negative
      //move past the space
    }
    else if(*This == 0)
    {
        return NOVALUE; // there is no value after the equal
    }
    if(Hex)
    {
        while(*This != 'x' && *This != 'X')
        {
            /* go to first number after the x */
            This++;
        }
        This++;
    }
    while(*This != 0 && *This != ' ' && *This != ',' && *This != '=')
    {
        if(!Hex)
        {
            if(*This >=48 && *This <= 57)
            {
                temp = temp * 10;
                temp += *This - 48;
                i++;
                if(i>32)
                {
                    return TOOBIG;//number too big
                }
            }
        }
        else
        {
            if((*This >=48 && *This <= 57) || (*This >=65 && *This <= 70) || (*This >= 97 && *This <= 102))
            {
                temp <<= 4;
                tempThis = *This;
                if(IsLetter(tempThis))
                {
                    MSC_LowercaseChar(&tempThis);
                    temp += (tempThis - 87);
                }
                else
                {
                    temp += (*This - 48);
                }
                i++;
                if(i>32)
                {
                    return TOOBIG;//number too big
                }
            }
        }
        This++;
    }
    *result = temp;
    return NoError;
}

/******************************************************************************/
/* MSC_StringAddEqual
 *
 * This function puts an equal sign between letters and a number value.
/******************************************************************************/
unsigned char MSC_StringAddEqual(unsigned char* Input)
{
    unsigned char i;
    unsigned char j =0;
    unsigned char firstnumber = 255;
    unsigned char temp[255];
    unsigned char NullPosition;
    for(i = 0; i<255; i++)
    {
        if(*Input == 0)
        {
            NullPosition = i;
            break;
        }
        Input++;
    }
    Input-=NullPosition;
    MSC_CleanBuffer(temp,NullPosition +1);
    i = 0;
    while(Input[i] != 0)
    {
            if(((Input[i] >= 48) && (Input[i] <= 57)) || (Input[i] == '-'))
            {
                firstnumber = i;
                break;
            }
            i++;
    }
    if(firstnumber == 255)
    {
        /* there is no number*/
        return FALSE;
    }
    for(j=0;j<firstnumber;j++)
    {
           temp[j] = Input[j];
    }
    temp[firstnumber] = '=';
    for(j = (firstnumber);j<255;j++)
    {
        temp[j+1] = Input[j];
        if(Input[j] == 0)
        {
            break;
        }
    }
    MSC_BufferCopy(temp,Input,NullPosition +1,0);
    return TRUE;
}

/******************************************************************************/
/* MSC_GetStringAfterComma
 *
 * This function resembles scanf. Use 0x...for hex number.
/******************************************************************************/
unsigned char MSC_GetStringAfterComma(unsigned char* This, unsigned char CommaNumber, unsigned char* result)
{
    unsigned char tempCommaNumber = 1;
    unsigned char count = 0;

    SCAN:while(*This != ',' && *This != 0)
    {
        /* go all the way to the first comma */
        This++;
    }
    if(*This == 0)
    {
        return NOCOMMA; // there is no equal
    }
    else if(*This != ',')
    {
        if(tempCommaNumber < CommaNumber)
        {
            tempCommaNumber++;
            This++;
            goto SCAN;
        }
    }
    if(*This == 0)
    {
        return NOCOMMA; // there is no equal
    }
    This++;
    if(*This == ' ')
    {
      This++;
      //move past the space
    }
    else if (*This == ',')
    {
         return DOUBLECOMMA; //double equal
    }
    else if(*This == 0)
    {
        return NOVALUE; // there is no value after the equal
    }
    while(*This != 0 && *This != ' ' && count < 10)
    {
        *result = *This;
        result++;
        This++;
        count++;
    }
    return NoError;
}

/******************************************************************************/
/* MSC_DB_Round
 *
 * This function rounds a double to the nearest whole number.
/******************************************************************************/
double MSC_DB_Round(double Input)
{
    double temp1;
    long temp;
    
    temp = (long) (Input + 0.5);
    temp1 = (double) temp;
    return temp1;
}

/******************************************************************************/
/* MSC_Round
 *
 * This function rounds a long to the specified number of places.
 * MSC_Round(11111,0) = 11111
 * MSC_Round(11111,1) = 11110
 * MSC_Round(11111,2) = 11100
/******************************************************************************/
long MSC_LG_Round(long Input,unsigned char places)
{
    long temp1,temp2,temp3,temp4, temp5, temp6, temp7;
    unsigned char negative = FALSE;
    long temp_Input;

    if(Input < 0)
    {
        negative = TRUE;
        temp_Input = Input * -1;
    }
    else
    {
        temp_Input = Input;
    }

    redo:
    if(!places)
    {
        return Input;
    }
    temp1   = pow(10,places);
    temp2   = temp_Input / temp1;
    temp3   = temp2 * temp1;
    temp4   = temp_Input - temp3;
    temp5   = temp1 >> 1;
    temp6   = temp1 - temp4;
    if(temp1 > temp_Input)
    {
        places--;
        goto redo;
    }
    if(temp5 >= temp6)
    {
        temp7 = temp_Input + temp6;
    }
    else
    {
        temp7 = temp3;
    }
    if(negative)
    {
        temp7 *= -1;
    }
    return temp7;
}

/******************************************************************************/
/* MSC_Scale
 *
 * This function scales a number from a,b to c,d.
/******************************************************************************/
long MSC_Scale(long Input, long InputRangeLow, long InputRangeHigh, long OutputRangeLow, long OutputRangeHigh)
{
    long DiffInput;
    long DiffOutput;
    double tempScale;
    DiffInput = InputRangeHigh - InputRangeLow;
    DiffOutput = OutputRangeHigh - OutputRangeLow;
    tempScale = (double)(DiffOutput) / (double)(DiffInput);
    return (long)(MSC_DB_Round( (double)(Input - InputRangeLow) * tempScale)) + OutputRangeLow;     
}

/******************************************************************************/
/* MSC_HEXtoBCD
 *
 * This function converts from Hex to BCD.
/******************************************************************************/
unsigned int MSC_HEXtoBCD(unsigned int input)
{
    unsigned int temp0;
    unsigned int temp1;
    unsigned int temp2;
    unsigned int temp3;
    unsigned int Value = input;
    
    temp3 = Value/1000;
    Value -= (temp3 * 1000);
    temp2 = Value / 100;
    Value -= (temp2 * 100);
    temp1 = Value / 10;
    Value -= (temp1 * 10);
    temp0 = Value;
    
    return (temp0 + (temp1 << 4) + (temp2 << 8) + (temp3 << 12)); 
}

/******************************************************************************/
/* MSC_BCDtoHEX
 *
 * This function converts from BCD to Hex.
/******************************************************************************/
unsigned int MSC_BCDtoHEX(unsigned int input)
{
    unsigned int temp0;
    unsigned int temp1;
    unsigned int temp2;
    unsigned int temp3;
    unsigned int Value = input;
    
    temp3 = Value >> 12;
    Value -= (temp3 << 12);
    temp2 = Value >> 8;
    Value -= (temp2 << 8);
    temp1 = Value >> 4;
    Value -= (temp1 << 4);
    temp0 = Value;
    
    return (temp0 + (temp1 * 10) + (temp2 * 100) + (temp3 * 1000));
}

/******************************************************************************/
/* MSC_BufferMatch
 *
 * This function checks to see if wach array match.
/******************************************************************************/
unsigned char MSC_BufferMatch(unsigned char* This, unsigned char* That, unsigned int size)
{
    unsigned int i;
    for (i=0; i < size; i++)
    {
        if(This[i] != That[i])
        {
            return FALSE;
        }
    }
    return TRUE;
}

/******************************************************************************/
/* MSC_TestFunctionPointer
 *
 * This function checks to see if a function pointer it is called correctly.
/******************************************************************************/
void MSC_TestFunctionPointer(void)
{
    Nop();
}

/******************************************************************************/
/* MSC_EndianInt
 *
 * This function converts a 2 byte number from big endian to little endian or
 * vice versa.
/******************************************************************************/
unsigned int MSC_EndianInt(unsigned int number)
{
    unsigned char temp1, temp2;

    temp1 = (unsigned char) (number & 0x00FF);
    temp2 = (unsigned char) (number & 0xFF00) >> 8;

    return (temp2 | (temp1 << 8));
}

/******************************************************************************/
/* MSC_EndianIntArray
 *
 * This function converts an array from big endian to little endian or
 * vice versa.
/******************************************************************************/
unsigned int MSC_EndianIntArray(unsigned char* buffer)
{
    unsigned char temp1, temp2;

    temp1 = *buffer;
    buffer++;
    temp2 = *buffer;

    return (temp1 | ((unsigned int) temp2 << 8));
}

/******************************************************************************/
/* MSC_EndianLong
 *
 * This function converts a 4 byte number from big endian to little endian or
 * vice versa.
/******************************************************************************/
unsigned long MSC_EndianLong(unsigned long number)
{
    unsigned char temp1, temp2, temp3, temp4;

    temp1 = (unsigned char) (number & 0x000000FF);
    temp2 = (unsigned char) (number & 0x0000FF00) >> 8;
    temp3 = (unsigned char) (number & 0x00FF0000) >> 16;
    temp4 = (unsigned char) (number & 0xFF000000) >> 24;

    return ((temp4 << 24) | (temp3 << 16) | (temp2 << 8) | temp1);
}

/******************************************************************************/
/* MSC_EndianLongArray
 *
 * This function converts an array from big endian to little endian or
 * vice versa.
/******************************************************************************/
unsigned long MSC_EndianLongArray(unsigned char* buffer)
{
    unsigned char temp1, temp2, temp3, temp4;

    temp1 = *buffer;
    buffer++;
    temp2 = *buffer;
    buffer++;
    temp3 = *buffer;
    buffer++;
    temp4 = *buffer;

    return ((unsigned long)temp1 | ((unsigned long) temp2 << 8) | ((unsigned long) temp3 << 16) | ((unsigned long) temp4 << 24));
}

/*-----------------------------------------------------------------------------/
 End of File
/-----------------------------------------------------------------------------*/