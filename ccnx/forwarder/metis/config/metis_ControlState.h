/*
 * Copyright (c) 2013-2015, Xerox Corporation (Xerox)and Palo Alto Research Center (PARC)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Patent rights are not granted under this agreement. Patent rights are
 *       available under FRAND terms.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL XEROX or PARC BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/**
 * @file metis_ControlState.h
 * @brief A control program for Metis using CLI commands
 *
 * Implements the state machine for the control program.  It takes a "writeRead" function
 * as part of the constructor.  This abstracts out the backend.  It could be a Portal from
 * metis_control program down to the forwarder or it could be an internal function within
 * metis.
 *
 * @author Marc Mosko, Palo Alto Research Center (Xerox PARC)
 * @copyright 2013-2015, Xerox Corporation (Xerox)and Palo Alto Research Center (PARC).  All rights reserved.
 */

#ifndef Metis_metis_control_h
#define Metis_metis_control_h

#include <parc/algol/parc_List.h>
#include <ccnx/transport/common/transport_MetaMessage.h>
#include <ccnx/forwarder/metis/config/metis_CommandParser.h>

struct metis_control_state;
typedef struct metis_control_state MetisControlState;

/**
 * metisControlState_Create
 *
 * Creates the global state for the MetisControl program.  The user provides the writeRead function
 * for sending and receiving the CCNxMetaMessage wrapping a CPIControlMessage.  For a CLI program, this
 * function would work over a CCNxSocket or CCNxComms.  For the baked-in CLI or configuration file
 * inside metis, it would make direct calls to MetisConfiguration.
 *
 * @param [in] userdata A closure passed back to the user when calling writeRead.
 * @param [in] writeRead The function to write then read configuration messages to Metis
 *
 * @return non-null The control state
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
MetisControlState *metisControlState_Create(void *userdata, CCNxMetaMessage * (*writeRead)(void *userdata, CCNxMetaMessage * msg));

/**
 * Destroys the control state, closing all network connections
 *
 * <#Paragraphs Of Explanation#>
 *
 * @param [<#in out in,out#>] <#name#> <#description#>
 *
 * @return <#value#> <#explanation#>
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
void metisControlState_Destroy(MetisControlState **statePtr);

/**
 * Registers a MetisCommandOps with the system.
 *
 * Each command has its complete command prefix in the "command" field.  RegisterCommand
 * will put these command prefixes in to a tree and then match what a user types against
 * the longest-matching prefix in the tree.  If there's a match, it will call the "execute"
 * function.
 *
 * @param [in] state An allocated MetisControlState
 * @param [in] command The command to register with the system
 *
 * Example:
 * @code
 *      static MetisCommandReturn
 *      metisControl_Root_Execute(MetisCommandParser *parser, MetisCommandOps *ops, PARCList *args)
 *      {
 *          printf("Root Command\n");
 *          return MetisCommandReturn_Success;
 *      }
 *
 *      static MetisCommandReturn
 *      metisControl_FooBar_Execute(MetisCommandParser *parser, MetisCommandOps *ops, PARCList *args)
 *      {
 *          printf("Foo Bar Command\n");
 *          return MetisCommandReturn_Success;
 *      }
 *
 *      const MetisCommandOps metisControl_Root = {
 *      .command = "", // empty string for root
 *      .init    = NULL,
 *      .execute = metisControl_Root_Execute
 *      };
 *
 *      const MetisCommandOps metisControl_FooBar = {
 *      .command = "foo bar", // empty string for root
 *      .init    = NULL,
 *      .execute = metisControl_FooBar_Execute
 *      };
 *
 *   void startup(void)
 *   {
 *      MetisControlState *state = metisControlState_Create("happy", "day");
 *      metisControlState_RegisterCommand(state, metisControl_FooBar);
 *      metisControlState_RegisterCommand(state, metisControl_Root);
 *
 *      // this executes "root"
 *      metisControlState_DispatchCommand(state, "foo");
 *      metisControlState_Destroy(&state);
 *  }
 * @endcode
 */
void metisControlState_RegisterCommand(MetisControlState *state, MetisCommandOps *command);

/**
 * Performs a longest-matching prefix of the args to the command tree
 *
 * The command tree is created with metisControlState_RegisterCommand.
 *
 * @param [in] state The allocated MetisControlState
 * @param [in] args  Each command-line word parsed to the ordered list
 *
 * @return MetisCommandReturn_Success the command was successful
 * @return MetisCommandReturn_Failure the command failed or was not found
 * @return MetisCommandReturn_Exit the command indicates that the interactive mode should exit
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
MetisCommandReturn metisControlState_DispatchCommand(MetisControlState *state, PARCList *args);

/**
 * Begin an interactive shell
 *
 * <#Paragraphs Of Explanation#>
 *
 * @param [<#in out in,out#>] <#name#> <#description#>
 *
 * @return <#value#> <#explanation#>
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
int metisControlState_Interactive(MetisControlState *state);

/**
 * Write then Read a CPI command
 *
 * @param [<#in out in,out#>] <#name#> <#description#>
 *
 * @return <#value#> <#explanation#>
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
CCNxMetaMessage *metisControlState_WriteRead(MetisControlState *state, CCNxMetaMessage *msg);

/**
 * Sets the Debug mode, which will print out much more information.
 *
 * Prints out much more diagnostic information about what metis-control is doing.
 * yes, you would make a MetisCommandOps to set and unset this :)
 *
 * @param [in] debugFlag true means to print debug info, false means to turn it off
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
void metisControlState_SetDebug(MetisControlState *state, bool debugFlag);

/**
 * Returns the debug state of MetisControlState
 *
 * <#Paragraphs Of Explanation#>
 *
 * @param [<#in out in,out#>] <#name#> <#description#>
 *
 * @return <#value#> <#explanation#>
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
bool metisControlState_GetDebug(MetisControlState *state);
#endif // Metis_metis_control_h
