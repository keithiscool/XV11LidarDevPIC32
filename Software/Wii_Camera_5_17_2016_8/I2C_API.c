#include <xc.h>
#include <stdbool.h>
#include <sys/attribs.h>
#include "I2C_API.h"

struct operator_values
{
    unsigned char slave_address;
    unsigned char data_address;
    unsigned char * data;
    unsigned char how_much_data;
    unsigned char data_index;
    unsigned char direction;
    unsigned char status;
};

static struct operator_values I2C_1_values = {0, 0, 0, 0, 0, 0, 1};
static struct operator_values I2C_2_values = {0, 0, 0, 0, 0, 0, 1};

void (*FunctionI2Cone)(void);
void (*FunctionI2Ctwo)(void);

bool SendI2Cone(unsigned char s_address, unsigned char * dat, unsigned char how_much)
{

    //see if a transmit or receive is in prograss
    if ((I2C_1_values.status == SUCCESS) || (I2C_1_values.status == FAILED))
    {
        //populate struct with needed data
        I2C_1_values.slave_address = s_address;
        I2C_1_values.data = dat;
        I2C_1_values.how_much_data = how_much;
        I2C_1_values.data_index = 0;
        I2C_1_values.direction = TRANSMIT;
        I2C_1_values.status = PENDING;
        FunctionI2Cone = &SendSlaveAddressI2Cone;
        I2C5CONbits.SEN = 1; // send start condition
        return true;
    } else
    {
        return false;
    }
}

bool SendI2Ctwo(unsigned char s_address, unsigned char * dat, unsigned char how_much)
{
        
    //see if a transmit or receive is in prograss
    if ((I2C_2_values.status == SUCCESS) || (I2C_2_values.status == FAILED))
    {
        //populate struct with needed data
        I2C_2_values.slave_address = s_address;
        I2C_2_values.data = dat;
        I2C_2_values.how_much_data = how_much;
        I2C_2_values.data_index = 0;
        I2C_2_values.direction = TRANSMIT;
        I2C_2_values.status = PENDING;
        FunctionI2Ctwo = &SendSlaveAddressI2Ctwo;
        I2C3CONbits.SEN = 1; // send start condition
        return true;
    } else
    {
        return false; //operation already in progress
    }
}

bool ReceiveI2Cone(unsigned char s_address, unsigned char d_address, unsigned char * dat, unsigned char how_much)
{
    //see if a transmit or receive is in prograss
    if ((I2C_1_values.status == SUCCESS) || (I2C_1_values.status == FAILED))
    {
        //populate struct with needed data
        I2C_1_values.slave_address = s_address;
        I2C_1_values.data_address = d_address;
        I2C_1_values.data = dat;
        I2C_1_values.how_much_data = how_much;
        I2C_1_values.data_index = 0;
        I2C_1_values.direction = RECEIVE;
        I2C_1_values.status = PENDING;
        FunctionI2Cone = &SendSlaveAddressI2Cone;
        I2C5CONbits.SEN = 1; // send start condition
        return true;
    } else
    {
        return false;
    }
}

bool ReceiveI2Ctwo(unsigned char s_address, unsigned char d_address, unsigned char * dat, unsigned char how_much)
{
    //see if a transmit or receive is in prograss
    if ((I2C_2_values.status == SUCCESS) || (I2C_2_values.status == FAILED))
    {
        //populate struct with needed data
        I2C_2_values.slave_address = s_address;
        I2C_2_values.data_address = d_address;
        I2C_2_values.data = dat;
        I2C_2_values.how_much_data = how_much;
        I2C_2_values.data_index = 0;
        I2C_2_values.direction = RECEIVE;
        I2C_2_values.status = PENDING;
        FunctionI2Ctwo = &SendSlaveAddressI2Ctwo;
        I2C3CONbits.SEN = 1; // send start condition
        return true;
    } else
    {
        return false; //operation already in progress
    }
}

void SendSlaveAddressI2Cone(void)
{
    I2C5TRN = I2C_1_values.slave_address;
    FunctionI2Cone = &SendDataAddressI2Cone;
}

void SendSlaveAddressI2Ctwo(void)
{
    I2C3TRN = I2C_2_values.slave_address;
    FunctionI2Ctwo = &SendDataAddressI2Ctwo;
}

void SendDataAddressI2Cone(void)
{
    if (I2C5STATbits.ACKSTAT == 0) //ack received
    {

        if (I2C_1_values.direction == RECEIVE)
        {
            I2C5TRN = I2C_1_values.data_address;
            FunctionI2Cone = &SendRestartI2Cone;
        } else if (I2C_1_values.direction == TRANSMIT)
        {
            if (I2C_1_values.data_index < I2C_1_values.how_much_data)
            {
                I2C5TRN = I2C_1_values.data[I2C_1_values.data_index];
                I2C_1_values.data_index++;
                FunctionI2Cone = &SendDataI2Cone;
            } else //all data has been sent
            {
                StopFunctionI2Cone();
                FunctionI2Cone = &SuccessFunctionI2Cone;
            }
        } else //neither transmit or receive
        {
            StopFunctionI2Cone();
            FunctionI2Cone = &FailFunctionI2Cone;
        }
    } else //nack received
    {
        StopFunctionI2Cone();
        FunctionI2Cone = &FailFunctionI2Cone;
    }
}

void SendDataAddressI2Ctwo(void)
{

    if (I2C3STATbits.ACKSTAT == 0) //ack received
    {

        if (I2C_2_values.direction == RECEIVE)
        {
            I2C3TRN = I2C_2_values.data_address;
            FunctionI2Ctwo = &SendRestartI2Ctwo;
        } else if (I2C_2_values.direction == TRANSMIT)
        {
            if (I2C_2_values.data_index < I2C_2_values.how_much_data)
            {
                I2C3TRN = I2C_2_values.data[I2C_2_values.data_index];
                I2C_2_values.data_index++;
                FunctionI2Ctwo = &SendDataI2Ctwo;
            } else //all data has been sent
            {
                StopFunctionI2Ctwo();
                FunctionI2Ctwo = &SuccessFunctionI2Ctwo;
            }

        } else //neither transmit or receive
        {
            StopFunctionI2Ctwo();
            FunctionI2Ctwo = &FailFunctionI2Ctwo;
        }
    } else //nack received
    {
        StopFunctionI2Ctwo();
        FunctionI2Ctwo = &FailFunctionI2Ctwo;
    }
}

void SendDataI2Cone(void)
{
    if (I2C5STATbits.ACKSTAT == 0) //ack received
    {
        //if index is less than how much data, send data and increment index
        if (I2C_1_values.data_index < I2C_1_values.how_much_data)
        {
            I2C5TRN = I2C_1_values.data[I2C_1_values.data_index];
            I2C_1_values.data_index++;
        } else //all data has been sent
        {
            StopFunctionI2Cone();
            FunctionI2Cone = &SuccessFunctionI2Cone;
        }
    } else //nack received
    {
        StopFunctionI2Cone();
        FunctionI2Cone = &FailFunctionI2Cone;
    }
}

void SendDataI2Ctwo(void)
{
    if (I2C3STATbits.ACKSTAT == 0) //ack received
    {
        //if index is less than how much data, send data and increment index
        if (I2C_2_values.data_index < I2C_2_values.how_much_data)
        {
            I2C3TRN = I2C_2_values.data[I2C_2_values.data_index];
            I2C_2_values.data_index++;
        } else //all data has been sent
        {
            StopFunctionI2Ctwo();
            FunctionI2Ctwo = &SuccessFunctionI2Ctwo;
        }
    } else //nack received
    {
        StopFunctionI2Ctwo();
        FunctionI2Ctwo = &FailFunctionI2Ctwo;
    }
}

void SendRestartI2Cone(void)
{
    I2C5CONbits.PEN = 1; //send stop
    FunctionI2Cone = &SendStartI2Cone;
}

void SendRestartI2Ctwo(void)
{
    I2C3CONbits.PEN = 1; //send stop
    FunctionI2Ctwo = &SendStartI2Ctwo;
}

void SendStartI2Cone(void)
{
    I2C5CONbits.SEN = 1; // send start condition
    FunctionI2Cone = &SendReadRequestI2Cone;
}

void SendStartI2Ctwo(void)
{
    I2C3CONbits.SEN = 1; // send start condition
    FunctionI2Ctwo = &SendReadRequestI2Ctwo;
}

void SendReadRequestI2Cone(void)
{
    I2C5TRN = (I2C_1_values.slave_address + 1);
    FunctionI2Cone = &FirstReceiveI2Cone;
}

void SendReadRequestI2Ctwo(void)
{
    I2C3TRN = (I2C_2_values.slave_address + 1);
    FunctionI2Ctwo = &FirstReceiveI2Ctwo;
}

void FirstReceiveI2Cone(void)
{
    if (I2C5STATbits.ACKSTAT == 0) //ack received
    {
        I2C5CONbits.RCEN = 1; // enable receive
        FunctionI2Cone = &ReceiveByteI2Cone;
    } else //nack received
    {
        StopFunctionI2Cone();
        FunctionI2Cone = &FailFunctionI2Cone;
    }
}

void FirstReceiveI2Ctwo(void)
{
    if (I2C3STATbits.ACKSTAT == 0) //ack received
    {
        I2C3CONbits.RCEN = 1; // enable receive
        FunctionI2Ctwo = &ReceiveByteI2Ctwo;
    } else //nack received
    {
        StopFunctionI2Ctwo();
        FunctionI2Ctwo = &FailFunctionI2Ctwo;
    }
}

void ReceiveByteI2Cone(void)
{
    I2C_1_values.data[I2C_1_values.data_index] = I2C5RCV;
    I2C_1_values.data_index++;
    if (I2C_1_values.data_index < I2C_1_values.how_much_data)
    {
        I2C5CONbits.ACKEN = 1; // send ACK
        FunctionI2Cone = &EnableReceiveI2Cone;
    } else
    {
        StopFunctionI2Cone();
        FunctionI2Cone = &SuccessFunctionI2Cone;
    }
}

void ReceiveByteI2Ctwo(void)
{
    I2C_2_values.data[I2C_2_values.data_index] = I2C3RCV;
    I2C_2_values.data_index++;
    if (I2C_2_values.data_index < I2C_2_values.how_much_data)
    {
        I2C3CONbits.ACKEN = 1; // send ACK
        FunctionI2Ctwo = &EnableReceiveI2Ctwo;
    } else
    {
        StopFunctionI2Ctwo();
        FunctionI2Ctwo = &SuccessFunctionI2Ctwo;
    }
}

void EnableReceiveI2Cone(void)
{
    I2C5CONbits.RCEN = 1; // enable receive
    FunctionI2Cone = &ReceiveByteI2Cone;
}

void EnableReceiveI2Ctwo(void)
{
    I2C3CONbits.RCEN = 1; // enable receive
    FunctionI2Ctwo = &ReceiveByteI2Ctwo;
}

void StopFunctionI2Cone(void)
{
    I2C5CONbits.PEN = 1; //send stop
}

void StopFunctionI2Ctwo(void)
{
    I2C3CONbits.PEN = 1; //send stop

}

void FailFunctionI2Cone(void)
{
    
    I2C_1_values.status = FAILED;
}

void FailFunctionI2Ctwo(void)
{
    I2C_2_values.status = FAILED;
}

void SuccessFunctionI2Cone(void)
{
    I2C_1_values.status = SUCCESS;
}

void SuccessFunctionI2Ctwo(void)
{
    I2C_2_values.status = SUCCESS;
}

unsigned char StatusI2Cone(void)
{
    return I2C_1_values.status;
}

unsigned char StatusI2Ctwo(void)
{
    return I2C_2_values.status;
}

void __ISR(_I2C_5_VECTOR, IPL1AUTO) I2CfiveHandler(void)
{

    if (I2C5STATbits.BCL == 1)
    {
        StopFunctionI2Cone();
        FunctionI2Cone = &FailFunctionI2Cone;
    } else
    {
        FunctionI2Cone();
    }
    IFS1CLR = _IFS1_I2C5MIF_MASK; // clear interrupt flag FIGURE OUT WHICH ONE ISTH ERIGHTONE FOR THE 5
}

void __ISR(_I2C_3_VECTOR, IPL1AUTO) I2CthreeHandler(void)
{
     

   
    if (I2C3STATbits.BCL == 1)
    {
        StopFunctionI2Ctwo();
        FunctionI2Ctwo = &FailFunctionI2Ctwo;
    } else
    {
        FunctionI2Ctwo();
    }
    IFS0CLR = _IFS0_I2C3MIF_MASK; // clear interrupt flag
}
