   /*******************************************************/
   /*      "C" Language Integrated Production System      */
   /*                                                     */
   /*            CLIPS Version 6.40  07/30/16             */
   /*                                                     */
   /*                USER FUNCTIONS MODULE                */
   /*******************************************************/

/*************************************************************/
/* Purpose:                                                  */
/*                                                           */
/* Principal Programmer(s):                                  */
/*      Gary D. Riley                                        */
/*                                                           */
/* Contributing Programmer(s):                               */
/*                                                           */
/* Revision History:                                         */
/*                                                           */
/*      6.24: Created file to seperate UserFunctions and     */
/*            EnvUserFunctions from main.c.                  */
/*                                                           */
/*      6.30: Removed conditional code for unsupported       */
/*            compilers/operating systems (IBM_MCW,          */
/*            MAC_MCW, and IBM_TBC).                         */
/*                                                           */
/*            Removed use of void pointers for specific      */
/*            data structures.                               */
/*                                                           */
/*************************************************************/

/***************************************************************************/
/*                                                                         */
/* Permission is hereby granted, free of charge, to any person obtaining   */
/* a copy of this software and associated documentation files (the         */
/* "Software"), to deal in the Software without restriction, including     */
/* without limitation the rights to use, copy, modify, merge, publish,     */
/* distribute, and/or sell copies of the Software, and to permit persons   */
/* to whom the Software is furnished to do so.                             */
/*                                                                         */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS */
/* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF              */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT   */
/* OF THIRD PARTY RIGHTS. IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY  */
/* CLAIM, OR ANY SPECIAL INDIRECT OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES */
/* WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN   */
/* ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF */
/* OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.          */
/*                                                                         */
/***************************************************************************/

#include "clips.h"
#include "../follower/condition.hpp"
#include "../follower/action.hpp"
#include "../follower/clips_function.hpp"

#include "../leader/leader_condition.hpp"
#include "../leader/leader_action.hpp"

void UserFunctions(Environment *);

/*********************************************************/
/* UserFunctions: Informs the expert system environment  */
/*   of any user defined functions. In the default case, */
/*   there are no user defined functions. To define      */
/*   functions, either this function must be replaced by */
/*   a function with the same name within this file, or  */
/*   this function can be deleted from this file and     */
/*   included in another file.                           */
/*********************************************************/



void UserFunctions(
  Environment *env)
  {
#if MAC_XCD
#pragma unused(env)
#endif


    // conditions
    AddUDF(env,"MetricInStateFor","l",2,2,"s",Condition::MetricInStateFor,"MetricInStateFor",NULL);
    AddUDF(env,"IsMetricEnabled","b",1,1,"s",Condition::IsMetricEnabled,"IsMetricEnabled",NULL);
    AddUDF(env,"NumMetricInStateFor","l",2,2,"sl",Condition::NumMetricInStateFor,"NumMetricInStateFor",NULL);
    //AddUDF(env,"OneMetrcAlarmingHigh","b",0,0,NULL,Condition::OneMetricAlarmingHigh,"OneMetricAlarmingHigh",NULL);
    //AddUDF(env,"AllMetricsOk","b",0,0,NULL,Condition::AllMetricsOk,"AllMetricsOk",NULL);

    // leader //
    AddUDF(env, "IsBatteryTooLow","b",1,1,"s",LeaderCondition::IsBatteryTooLow,"IsBatteryTooLow",NULL);
    AddUDF(env, "IsFollowerBatteryTooLow","m",0,0,NULL,LeaderCondition::IsFollowerBatteryTooLow,"IsFollowerBatteryTooLow",NULL);

    // actions
    AddUDF(env, "ChangeTimeReport","v",1,1,"l",Action::ChangeTimeReport,"ChangeTimeReport",NULL);
    AddUDF(env, "ChangeTimeTests","v",1,1,"l",Action::ChangeTimeTests,"ChangeTimeTests",NULL);
    AddUDF(env, "ChangeTimeLatency","v",1,1,"l",Action::ChangeTimeLatency,"ChangeTimeLatency",NULL);
    AddUDF(env, "DisableMetrics","v",1,1,"m",Action::DisableMetrics,"DisableMetrics",NULL);
    AddUDF(env, "EnableMetrics", "v",1,1,"m",Action::EnableMetrics,"EnableMetrics",NULL);
    AddUDF(env, "SetLeaderAdequacy","v",1,1,"l",Action::SetLeaderAdequacy,"SetLeaderAdequacy",NULL);

    // leader //
    AddUDF(env, "ChangeTimeReportLeader","v",2,2,"ls",LeaderAction::ChangeTimeReportLeader,"ChangeTimeReportLeader",NULL);
    AddUDF(env, "SendChangeServer","v",0,0,NULL,LeaderAction::SendChangeServer,"SendChangeServer",NULL);
    AddUDF(env, "SendDisableMetrics","v",1,10,"ym",LeaderAction::SendDisableMetrics,"SendDisableMetrics",NULL);
    AddUDF(env, "SendEnableMetrics", "v",1,10,"ym",LeaderAction::SendEnableMetrics,"SendEnableMetrics",NULL);

    // helper
    AddUDF(env, "GetTimeReportMean", "l",5,5,"l",ClipsFunction::GetTimeReportMean,"GetTimeReportMean",NULL);
    AddUDF(env, "GetNumActiveMetrics",NULL,0,0,"l",ClipsFunction::GetNumActiveMetrics,"GetNumActiveMetrics",NULL);
    AddUDF(env, "LoadFactsLeader","v",0,0,NULL,ClipsFunction::LoadFactsLeader,"LoadFactsLeader",NULL);
    AddUDF(env, "LoadFactsFollower","v",0,0,NULL,ClipsFunction::LoadFactsFollower,"LoadFactsFollower",NULL);
    AddUDF(env, "GetTimeReport", "l",3,3,"sl",ClipsFunction::GetTimeReport,"GetTimeReport",NULL);
}
