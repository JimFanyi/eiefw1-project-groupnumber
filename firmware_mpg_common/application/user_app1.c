/**********************************************************************************************************************
File: user_app1.c                                                                

Description:
Provides a Tera-Term driven system to display, read and write an LED command list.

------------------------------------------------------------------------------------------------------------------------
API:

Public functions:
None.

Protected System functions:
void UserApp1Initialize(void)
Runs required initialzation for the task.  Should only be called once in main init section.

void UserApp1RunActiveState(void)
Runs current task state.  Should only be called once in main loop.


**********************************************************************************************************************/

#include "configuration.h"

/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_"
***********************************************************************************************************************/
/* New variables */
volatile u32 G_u32UserApp1Flags;                       /* Global state flags */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern volatile u32 G_u32SystemFlags;                  /* From main.c */
extern volatile u32 G_u32ApplicationFlags;             /* From main.c */

extern volatile u32 G_u32SystemTime1ms;                /* From board-specific source file */
extern volatile u32 G_u32SystemTime1s;                 /* From board-specific source file */

extern u8 G_au8DebugScanfBuffer[DEBUG_SCANF_BUFFER_SIZE]; /* From debug.c */
extern u8 G_u8DebugScanfCharCount;                        /* From debug.c */


/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "UserApp1_" and be declared as static.
***********************************************************************************************************************/
static fnCode_type UserApp1_StateMachine;            /* The state machine function pointer */
//static u32 UserApp1_u32Timeout;                      /* Timeout counter used across states */


/**********************************************************************************************************************
Function Definitions
**********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/* Public functions                                                                                                   */
/*--------------------------------------------------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------------------------------------------------*/
/* Protected functions                                                                                                */
/*--------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
Function: UserApp1Initialize

Description:
Initializes the State Machine and its variables.

Requires:
  -

Promises:
  - 
*/
void UserApp1Initialize(void)
{
  u8 au8UserApp1Start1[] = "LED program task started\n\r";
  
  /* Turn off the Debug task command processor and announce the task is ready */
  DebugSetPassthrough();
  DebugPrintf(au8UserApp1Start1);
  
    /* If good initialization, set state to Idle */
  if( 1 )
  {
    UserApp1_StateMachine = UserApp1SM_Idle;
  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */
    UserApp1_StateMachine = UserApp1SM_FailedInit;
  }

} /* end UserApp1Initialize() */

  
/*----------------------------------------------------------------------------------------------------------------------
Function UserApp1RunActiveState()

Description:
Selects and runs one iteration of the current state in the state machine.
All state machines have a TOTAL of 1ms to execute, so on average n state machines
may take 1ms / n to execute.

Requires:
  - State machine function pointer points at current state

Promises:
  - Calls the function to pointed by the state machine function pointer
*/
void UserApp1RunActiveState(void)
{
  UserApp1_StateMachine();

} /* end UserApp1RunActiveState */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Private functions                                                                                                  */
/*--------------------------------------------------------------------------------------------------------------------*/
static void PrintStartMenu(void)
{
  u8 au8Star[50];
  u8 au8Menu[] = "Press 1 to program LED command sequence\n\rPress 2 to show current USER program\n\r";
  u8 u8i;
  
  for(u8i = 0; u8i < 47; u8i++)
  {
    au8Star[u8i] = '*';
  }
  au8Star[47] = '\n';
  au8Star[48] = '\r';
  au8Star[49] = '\0';
  
  DebugPrintf(au8Star);
  DebugPrintf(au8Menu);
  DebugPrintf(au8Star);
  
}

static void PrintMenu1(void)
{
  u8 au8Instructions[] = "Enter commands as LED-ONTIME-OFFTIME and press Enter\n\rTime is in milliseconds, max 100 commands\n\rLED colors: R,O,Y,G,C,B,P,W\n\rExample: R-100-200(Red on at100ms and off at 200ms)\n\rPress Enter on blank line to end\n\r";
  DebugPrintf("\n\r\n\r");
  DebugPrintf(au8Instructions);
}

static u8 ReadCommand(void)
{
  static u8 au8CommandInput[1];
  u8 au8CommandUseful[1];
  
  DebugScanf(au8CommandInput);
  
  if (au8CommandInput[0] != '\0')
  {
    au8CommandUseful[0] = au8CommandInput[0];
    return au8CommandUseful[0];
  }
  else
    return 0;
}

static void ReadProgram(void)
{
  u8 au8ProgramInput[1];
  static u8 u8StepCounter = 1;
  static u8 u8EnterCounter = 0;
  static u32 u32OFFTime = 0;
  static u32 u32ONTime = 0;
  static bool bHasONTIMEBeenInputted = FALSE;
  static bool bHasOFFTIMEBeenInputted = FALSE;
  
  DebugScanf(au8ProgramInput);
  
  if (au8ProgramInput[0] != '\0')
  {
    
    if(au8ProgramInput[0] == 'R' && u8StepCounter == 1)
    {
      bHasONTIMEBeenInputted = FALSE;
      bHasOFFTIMEBeenInputted = FALSE;
      u8EnterCounter = 0;
      u8StepCounter++;
    }
    else if(au8ProgramInput[0] == 'O' && u8StepCounter == 1)
    {
      bHasONTIMEBeenInputted = FALSE;
      bHasOFFTIMEBeenInputted = FALSE;
      u8EnterCounter = 0;
    }
    else if(au8ProgramInput[0] == 'Y' && u8StepCounter == 1)
    {
      bHasONTIMEBeenInputted = FALSE;
      bHasOFFTIMEBeenInputted = FALSE;
      u8EnterCounter = 0;
    }
    else if(au8ProgramInput[0] == 'G' && u8StepCounter == 1)
    {
      bHasONTIMEBeenInputted = FALSE;
      bHasOFFTIMEBeenInputted = FALSE;
      u8EnterCounter = 0;
    }
    else if(au8ProgramInput[0] == 'C' && u8StepCounter == 1)
    {
      u8EnterCounter = 0;
    }
    else if(au8ProgramInput[0] == 'B' && u8StepCounter == 1)
    {
      u8EnterCounter = 0;
    }
    else if(au8ProgramInput[0] == 'P' && u8StepCounter == 1)
    {
      u8EnterCounter = 0;
    }
    else if(au8ProgramInput[0] == 'W' && u8StepCounter == 1)
    {
      u8EnterCounter = 0;
    }
    else if(au8ProgramInput[0] == '-' && u8StepCounter == 2)
    {
      u8EnterCounter = 0;
      u8StepCounter++;
    }
    else if(au8ProgramInput[0] >= '0' && au8ProgramInput[0] <= '9'&& u8StepCounter == 3)
    {
      
      u32ONTime = u32ONTime*10 + au8ProgramInput[0] - 48;
      bHasONTIMEBeenInputted = TRUE;  
      u8EnterCounter = 0;
    }
    else if(au8ProgramInput[0] == '-' && bHasONTIMEBeenInputted)
    {
      u8StepCounter++;
      u8EnterCounter = 0;
    }
    else if(au8ProgramInput[0] >= '0' && au8ProgramInput[0] <= '9' && u8StepCounter == 4)
    {
     
      u32OFFTime = u32OFFTime*10 + au8ProgramInput[0] - 48;
      u8EnterCounter = 0;
      bHasOFFTIMEBeenInputted = TRUE;
    }
    else if(au8ProgramInput[0] == '\r' && bHasOFFTIMEBeenInputted)
    {
      u8EnterCounter++;
      if(u8EnterCounter == 1)
      {
        DebugPrintf("\n\r");
        u8StepCounter = 1;
        u32OFFTime = 0;
        u32ONTime = 0;
        
      }
      if(u8EnterCounter == 2)
      {
        DebugPrintNumber(u32OFFTime);
        u8EnterCounter = 0;
      }
    }
    
    
    
  }
  
  
}


/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/

/*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for input */

static void UserApp1SM_Idle(void)
{
  static bool bIsProgramStarting = TRUE;
  static bool bHasMenu12BeenShowed = FALSE;
  
  if (bIsProgramStarting)
  {
    PrintStartMenu();
    bIsProgramStarting = FALSE;
  }
  if (bHasMenu12BeenShowed == FALSE)
  {
    if(ReadCommand() == '1' && bHasMenu12BeenShowed == FALSE)
    {
      PrintMenu1();
      bHasMenu12BeenShowed = TRUE;
    }
    else if(ReadCommand() == '2'&& bHasMenu12BeenShowed == FALSE)
    {
      DebugPrintf("NO");
      bHasMenu12BeenShowed = TRUE;
    }
  }
  
  
  if(bHasMenu12BeenShowed)
  {
    ReadProgram();
  }
} /* end UserApp1SM_Idle() */
                      
            
#if 0
/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserApp1SM_Error(void)          
{
  
} /* end UserApp1SM_Error() */
#endif


/*-------------------------------------------------------------------------------------------------------------------*/
/* State to sit in if init failed */
static void UserApp1SM_FailedInit(void)          
{
    
} /* end UserApp1SM_FailedInit() */


/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
