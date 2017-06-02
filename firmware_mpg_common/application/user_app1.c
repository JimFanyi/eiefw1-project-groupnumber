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

/*--------------------------------------------------------------------------------------------------------------------
Function: PrintStartMenu

Description:When the program starts, show a menu with two choices.

Requires: Only show the menu when the program starts.

Promises: A menu with two choices will be shown when the program starts.
*/
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

/*--------------------------------------------------------------------------------------------------------------------
Function: PrintMenu1

Description: When the user input 1,show clear instruction of how to enter a string

Requires: The user choosed 1 when the program starts

Promises: A clear instruction of how to enter a string will be shown.
*/
static void PrintMenu1(void)
{
  u8 au8Instructions[] = 
     "Enter commands as LED-ONTIME-OFFTIME and press Enter\n\rTime is in milliseconds, max 100 commands\n\rLED colors: R,O,Y,G,C,B,P,W\n\rExample: R-100-200(Red on at100ms and off at 200ms)\n\rPress Enter on blank line to end\n\r";
  DebugPrintf("\n\r\n\r");
  DebugPrintf(au8Instructions);
}

/*--------------------------------------------------------------------------------------------------------------------
Function: ReadCommand

Description: Read the command '1' or '2'

Requires: no requires.

Promises: The '1' or '2' the user inputted will be read.
*/
static u8 ReadCommand(void)
{
  static u8 au8CommandInput[1] = '\0';
  u8 au8CommandUseful[1];
  
  DebugScanf(au8CommandInput);
  
  if (au8CommandInput[0] != '\0')
  {
    au8CommandUseful[0] = au8CommandInput[0];
    au8CommandInput[0] = '\0';
    return au8CommandUseful[0];
  }
  else
    return 0;
}

/*--------------------------------------------------------------------------------------------------------------------
Function: ReadProgram.

Description: 
Read the program the user has inputted, and determine if the program is correct. If correct, 
send the information to the command list. If wrong, show the error information and give the correct example.

Requires:
-The menu for the user to choose '1' or '2' has been shown.

Promises:
- As long as the program is correct, the information will be send to the command list.
- If the program is wrong, an error information will be given to the user.
*/

static bool ReadProgram(void)
{
  u8 au8ProgramInput[1] = "\0";
  /*Counter Definition*/
  static u8 u8StepCounter = 1;   //The counter of current step, to determine what command should be inputted.
  static u8 u8EnterCounter = 0;  //The counter of the times the enter has been pressed, to determine whether the user programming has endded.
  static u8 au8flag[6] = {0};
  static u8 u8InputCounter = 1;
  /*ONTIME & OFFTIME STORAGE*/
  static u32 u32OFFTime = 0;    //a temp storage to store the ontime the user has entered.
  static u32 u32ONTime = 0;     //a temp storage to store the offtime the user has entered. 
  
  
  /*bool variable definition*/
  static bool bHasONTIMEBeenInputted = FALSE;
  static bool bHasOFFTIMEBeenInputted = FALSE;
  static bool bTheFirstEnterHasBeenPressed = FALSE;
  static bool bColorEntered = FALSE;
  static bool bEnterCorrect = TRUE; //a bool variable to check if the inputments is right.
  static bool bColorEnterCorrect = TRUE;
  static bool bONTimeEnterCorrect = TRUE;
  static bool bOFFTimeEnterCorrect = TRUE;
  static bool bLetterWrong = FALSE;
  static bool bNumberWrong = FALSE;
  bool bProgrammingEndded = FALSE;
  
  static u8 u8Color = 0;
  
  LedCommandType CommandInfo; //a temp struct to store the command info.
  
  DebugScanf(au8ProgramInput); //Monitor the input stream every 1ms.
  

 /*When the user has inputted something, then do the tasks in the "if".*/
  if (au8ProgramInput[0] != '\0')
  {
   /*The correct command include color, '-', ontime and off time, and they must be arranged in a certain order.*/
    if(au8ProgramInput[0] != '\r')
    {
      u8EnterCounter = 0;
      bTheFirstEnterHasBeenPressed = FALSE;
    }
    if(au8ProgramInput[0] == 'R' && u8StepCounter == 1)
    {
      /*--------------------------------------------------------------------------------------
      When the user inputs the color, we regard it as the start of a command, so we initialize 
      all the counters and bool variables that may have been changed in the former command.
      */
      if(bColorEnterCorrect)
      {
        bHasONTIMEBeenInputted = FALSE;
        bHasOFFTIMEBeenInputted = FALSE;
        u32OFFTime = 0;
        u32ONTime = 0;
        u8EnterCounter = 0;
        bEnterCorrect = TRUE;
      
      }
      
      u8StepCounter++;
    }
    
    else if(au8ProgramInput[0] == 'O' && u8StepCounter == 1)
    {
      if(bColorEnterCorrect)
      {
        bHasONTIMEBeenInputted = FALSE;
        bHasOFFTIMEBeenInputted = FALSE;
        u32OFFTime = 0;
        u32ONTime = 0;
        u8EnterCounter = 0;
        bEnterCorrect = TRUE;
      
      }
      
      u8StepCounter++;
    }
    
    else if(au8ProgramInput[0] == 'Y' && u8StepCounter == 1)
    {
     if(bColorEnterCorrect)
      {
        bHasONTIMEBeenInputted = FALSE;
        bHasOFFTIMEBeenInputted = FALSE;
        u32OFFTime = 0;
        u32ONTime = 0;
        u8EnterCounter = 0;
        bEnterCorrect = TRUE;
      
      }
      
      u8StepCounter++;
    }
    else if(au8ProgramInput[0] == 'G' && u8StepCounter == 1)
    {
     if(bColorEnterCorrect)
      {
        bHasONTIMEBeenInputted = FALSE;
        bHasOFFTIMEBeenInputted = FALSE;
        u32OFFTime = 0;
        u32ONTime = 0;
        u8EnterCounter = 0;
        bEnterCorrect = TRUE;
      
      }
      
      u8StepCounter++;
    }
    
    else if(au8ProgramInput[0] == 'C' && u8StepCounter == 1)
    {
      if(bColorEnterCorrect)
        {
          bHasONTIMEBeenInputted = FALSE;
          bHasOFFTIMEBeenInputted = FALSE;
          u32OFFTime = 0;
          u32ONTime = 0;
          u8EnterCounter = 0;
          bEnterCorrect = TRUE;
        
      }
      
      u8StepCounter++;
    }
    
    else if(au8ProgramInput[0] == 'B' && u8StepCounter == 1)
    {
    if(bColorEnterCorrect)
      {
        bHasONTIMEBeenInputted = FALSE;
        bHasOFFTIMEBeenInputted = FALSE;
        u32OFFTime = 0;
        u32ONTime = 0;
        u8EnterCounter = 0;
        bEnterCorrect = TRUE;
      
      }
      
      u8StepCounter++;
    }
    
    else if(au8ProgramInput[0] == 'P' && u8StepCounter == 1)
    {
      if(bColorEnterCorrect)
      {
        bHasONTIMEBeenInputted = FALSE;
        bHasOFFTIMEBeenInputted = FALSE;
        u32OFFTime = 0;
        u32ONTime = 0;
        u8EnterCounter = 0;
        bEnterCorrect = TRUE;
      
      }
      
      u8StepCounter++;
    }
    
    else if(au8ProgramInput[0] == 'W' && u8StepCounter == 1 )
    {
      if(bColorEnterCorrect)
      {
        bHasONTIMEBeenInputted = FALSE;
        bHasOFFTIMEBeenInputted = FALSE;
        u32OFFTime = 0;
        u32ONTime = 0;
        u8EnterCounter = 0;
        bEnterCorrect = TRUE;
      
      }
      
      u8StepCounter++;
    }
    else if(au8ProgramInput[0] != ' ' && au8ProgramInput[0]!='\r' && au8ProgramInput[0]!='-' && u8StepCounter == 1)
    {
      bLetterWrong = TRUE;
      bEnterCorrect = FALSE;
      u8StepCounter++;
    }
    
    else if(au8ProgramInput[0] == '-' && u8StepCounter == 2)
    {
      u8EnterCounter = 0;
      u8StepCounter++;
    }
    
    else if(au8ProgramInput[0] >= '0' && au8ProgramInput[0] <= '9'&& u8StepCounter == 3)
    { 
      if(bONTimeEnterCorrect)
      {
        u32ONTime = u32ONTime*10 + au8ProgramInput[0] - 48;
      }
      
      bHasONTIMEBeenInputted = TRUE;
      u8EnterCounter = 0;
    }
    else if(au8ProgramInput[0] != ' ' && au8ProgramInput[0]!='\r' && au8ProgramInput[0]!='-' && u8StepCounter == 3)
    {
      bNumberWrong = TRUE;
      u8StepCounter++;
      bEnterCorrect = FALSE;
    }
    
    else if(au8ProgramInput[0] == '-' && bHasONTIMEBeenInputted)
    {
      u8StepCounter++;
      u8EnterCounter = 0;
    }
    
    else if(au8ProgramInput[0] >= '0' && au8ProgramInput[0] <= '9' && u8StepCounter == 4)
    {
      if(bOFFTimeEnterCorrect)
      {
        u32OFFTime = u32OFFTime*10 + au8ProgramInput[0] - 48;
      }
      
      u8EnterCounter = 0;
      bHasOFFTIMEBeenInputted = TRUE;
    }  
    
    else if(au8ProgramInput[0] != ' ' && au8ProgramInput[0]!='\r' && u8StepCounter == 4)
    {
      bNumberWrong = TRUE;
      u8StepCounter++;
       bEnterCorrect = FALSE;
    }
    
    
    
    else if(au8ProgramInput[0] == '\r')
    {
      if(bHasOFFTIMEBeenInputted && bEnterCorrect || bTheFirstEnterHasBeenPressed)
      {
        u8EnterCounter++;
        if(u8EnterCounter == 1)
        {
          DebugPrintf("\n\r");
          //LedDisplayAddCommand(USER_LIST,CommandInfo);
          u8StepCounter = 1;
          bTheFirstEnterHasBeenPressed = TRUE;
          bHasOFFTIMEBeenInputted = FALSE;
          bProgrammingEndded = FALSE;
          u8InputCounter++;
          DebugPrintNumber(u8InputCounter);
          DebugPrintf(":");
         
        }
        
        
        
        if(u8EnterCounter == 2)
        {
          DebugPrintNumber(u32OFFTime);
          DebugPrintf("ProgrammingEndded\n\r");
          u8EnterCounter = 0;
          u8InputCounter = 1;
          bTheFirstEnterHasBeenPressed = FALSE;
          bProgrammingEndded = TRUE;
        }
       }
      else
      {
        DebugPrintf("\n\rInvalid Command!\n\rDetails:\n\r");
        if(au8flag[0] == 1)
        {
          DebugPrintf("Inputted ' ' before color\n\r");
          bColorEnterCorrect = TRUE;
          au8flag[0] = 0;
        }
        
        if(au8flag[1] == 1)
        {
          DebugPrintf("Inputted ' ' after color\n\r");
          bColorEnterCorrect = TRUE;
          au8flag[1] = 0;
        }
        
        if(au8flag[2] == 1)
        {
          DebugPrintf("Inputted ' ' before ONTime\n\r");
          bONTimeEnterCorrect = TRUE;
          au8flag[2] = 0;
            
        }
        
        if(au8flag[3] == 1)
        {
          DebugPrintf("Inputted ' ' after ONTime\n\r");
          bONTimeEnterCorrect = TRUE;
          au8flag[3] = 0;
        }
        
        if(au8flag[4] == 1)
        {
          DebugPrintf("Inputted ' ' before OFFTime\n\r");
          bONTimeEnterCorrect = TRUE;
          au8flag[4] = 0;
        }
        
        if(bLetterWrong)
        {
          DebugPrintf("Color must be R,O,Y,G,C,B,P,W\n\r");
          bLetterWrong = FALSE;
        }
        
        if(bNumberWrong)
        {
          DebugPrintf("ONTime&OFFTime Include invalid command\n\r");
          bNumberWrong = FALSE;
        }
        
        
        
        DebugPrintf("\n\r");
        DebugPrintNumber(u8InputCounter);
        DebugPrintf(":");
        
        
        u8StepCounter = 1;
      }
    }
    
    else if(au8ProgramInput[0] == ' ')
    {
      if(u8StepCounter == 1)
      {
        au8flag[0] = 1;
        bColorEnterCorrect = FALSE;
      }
      
      if(u8StepCounter == 2)
      {
        au8flag[1] = 1;
        bColorEnterCorrect = FALSE;
      }
      
      if(u8StepCounter == 3 && !bHasONTIMEBeenInputted)
      {
        au8flag[2] = 1;
        bONTimeEnterCorrect = FALSE;
        bEnterCorrect = FALSE;
      }
      if(bHasONTIMEBeenInputted && u8StepCounter == 3)
      {
        au8flag[3] = 1;
        bONTimeEnterCorrect = FALSE;
        bEnterCorrect = FALSE;
      }
       
      if(u8StepCounter == 4 && !bHasOFFTIMEBeenInputted)
      {
        au8flag[4] = 1;
        bOFFTimeEnterCorrect = FALSE;
        bEnterCorrect = FALSE;
      }
      
      
    }
    

    else
    {
      bEnterCorrect = FALSE;
      u8StepCounter = 1;
    }
    

    
    return bProgrammingEndded;
    
    
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
  static bool bHasMenu12BeenShowed = FALSE; //A bool to show if the choice list has been shown
  static bool bProgrammingCompleted = FALSE;
  static bool bMenu1HasBeenShowed = FALSE;
  static bool bHasMenu2BeenShowed = FALSE;
  static u8 au8Command[1];
  
  if (bIsProgramStarting || bProgrammingCompleted)
  {
    PrintStartMenu();
    bIsProgramStarting = FALSE;
    bHasMenu12BeenShowed = FALSE;//A bool equals to bMenu1HasBeenShowed || bMenu2HasBeenShowed;
    bProgrammingCompleted = FALSE;
    bMenu1HasBeenShowed = FALSE;
    bHasMenu2BeenShowed = FALSE;
  }
  if (bHasMenu12BeenShowed == FALSE || bHasMenu2BeenShowed)
  {
    au8Command[0] = ReadCommand();
    if(au8Command[0] == '1' && bHasMenu12BeenShowed == FALSE)
    {
      PrintMenu1();
      DebugPrintf("\n\r1:");
      bHasMenu12BeenShowed = TRUE;
      bMenu1HasBeenShowed = TRUE;
      bProgrammingCompleted = FALSE;
    }
    else if(au8Command[0] == '2' && bHasMenu12BeenShowed == FALSE)
    {
      DebugPrintf("NO");
      bHasMenu12BeenShowed = TRUE;
      bHasMenu2BeenShowed = TRUE;
    }
    else if(au8Command[0] == '\r' && bHasMenu2BeenShowed)
    {

      bIsProgramStarting = TRUE;
      DebugPrintf("\n\r\n\r");
    }
  }
  
  
  if(bMenu1HasBeenShowed)
  {
   bProgrammingCompleted = ReadProgram();
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
