/****************************************************************************************
 * Filename:	test-agents.h
 * Author:		Craig Paton
 * Date:			Apr 2004
 * 
 * Implemenation of agents that are used for testing the platforms functionality
 * **************************************************************************************/

#ifndef __TESTS_TEST_AGENTS_H__
#define __TESTS_TEST_AGENTS_H__

void bootAgent(char* name);
void AMSModifyAgent(char* name);
void AMSSearchAgent(char* name);
void DFRegAgent(char* name);
void DFModifyAgent(char* name);
void DFSearchAgent(char* name);
void agent(char* name);
void serverAgent(char* name);

#endif
