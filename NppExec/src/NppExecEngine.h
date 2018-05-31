/*
This file is part of NppExec
Copyright (C) 2013 DV <dvv81 (at) ukr (dot) net>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _npp_exec_engine_h_
#define _npp_exec_engine_h_
//---------------------------------------------------------------------------
#include "base.h"
#include "NppExec.h"
#include <memory>
#include <map>
#include <list>
#include <cassert>

LRESULT CALLBACK nppPluginWndProc(HWND, UINT, WPARAM, LPARAM);

char*  SciTextFromLPCTSTR(LPCTSTR pText, HWND hSci, int* pnLen = NULL); // finally call SciTextDeleteResultPtr
LPTSTR SciTextToLPTSTR(const char* pSciText, HWND hSci); // finally call SciTextDeleteResultPtr

#ifdef UNICODE
#define SciTextDeleteResultPtr(ptrResult, ptrOriginal) if (ptrResult) delete[] ptrResult
#else
#define SciTextDeleteResultPtr(ptrResult, ptrOriginal) if (ptrResult && ptrResult != ptrOriginal) delete[] ptrResult
#endif

class CScriptEngine : public IScriptEngine
{
    public:
        enum eCmdType {
            CMDTYPE_COLLATERAL_FORCED = -2,
            CMDTYPE_COMMENT_OR_EMPTY = -1,
            CMDTYPE_UNKNOWN = 0,

            CMDTYPE_NPPEXEC = 1,
            CMDTYPE_NPPOPEN,
            CMDTYPE_NPPRUN,
            CMDTYPE_NPPSAVE,
            CMDTYPE_NPPSWITCH,
            CMDTYPE_CLS,
            CMDTYPE_CD,
            CMDTYPE_DIR,
            CMDTYPE_NPPSAVEALL,
            CMDTYPE_CONLOADFROM,
            CMDTYPE_CONSAVETO,
            CMDTYPE_ECHO,
            CMDTYPE_NPEDEBUGLOG,
            CMDTYPE_SET,
            CMDTYPE_UNSET,
            CMDTYPE_NPENOEMPTYVARS,
            CMDTYPE_SELSAVETO,
            CMDTYPE_NPPCLOSE,
            CMDTYPE_INPUTBOX,
            CMDTYPE_NPPCONSOLE,
            CMDTYPE_ENVSET,
            CMDTYPE_ENVUNSET,
            CMDTYPE_NPECONSOLE,
            CMDTYPE_SELLOADFROM,
            CMDTYPE_SELSETTEXT,
            CMDTYPE_SELSETTEXTEX,
            CMDTYPE_NPECMDALIAS,
            CMDTYPE_NPPSENDMSG,
            CMDTYPE_SCISENDMSG,
            CMDTYPE_NPPSENDMSGEX,
            CMDTYPE_NPPMENUCOMMAND,
            CMDTYPE_CONCOLOUR,
            CMDTYPE_CONFILTER,
            CMDTYPE_IF,
            CMDTYPE_LABEL,
            CMDTYPE_GOTO,
            CMDTYPE_NPPSAVEAS,
            CMDTYPE_ELSE,
            CMDTYPE_ENDIF,
            CMDTYPE_PROCSIGNAL,
            CMDTYPE_SLEEP,
            CMDTYPE_NPEQUEUE,
            CMDTYPE_SCIFIND,
            CMDTYPE_SCIREPLACE,
            CMDTYPE_TEXTLOADFROM,
            CMDTYPE_TEXTSAVETO,
            CMDTYPE_NPPSETFOCUS,
            CMDTYPE_CLIPSETTEXT,

            CMDTYPE_TOTAL_COUNT
        };

        enum eOnOffParam {
            PARAM_EMPTY   = -1,
            PARAM_OFF     = 0,
            PARAM_ON      = 1,
            PARAM_KEEP    = 2,
            PARAM_ENABLE  = 3,
            PARAM_DISABLE = 4,
            PARAM_UNKNOWN = 10
        };

        enum eCmdResult {
            CMDRESULT_INVALIDPARAM = -1, // error in command's parameter(s)
            CMDRESULT_FAILED = 0,        // error while executing the command
            CMDRESULT_SUCCEEDED = 1,     // OK, success
        };

        enum eIfState {
            IF_NONE = 0,   // not under IF
            IF_EXECUTING,  // IF-condition is true, executing lines under the IF
            IF_WANT_ELSE,  // IF-condition is false, want another ELSE (or ENDIF)
            IF_WANT_ENDIF, // done with IF or ELSE, want ENDIF
            IF_MAYBE_ELSE, // special case: ELSE *may* appear after GOTO
            IF_WANT_SILENT_ENDIF // special case: nested IF in a skipped condition block
        };

        enum eNppExecCmdPrefix {
            CmdPrefixNone = 0,
            CmdPrefixCollateralOrRegular,
            CmdPrefixCollateralForced
        };

        enum eGetCmdTypeFlags {
            ctfUseLogging   = 0x01,
            ctfIgnorePrefix = 0x02
        };

    public:
        typedef eCmdResult (*EXECFUNC)(CScriptEngine* pEngine, const tstr& params);

        // For each of the Do-methods:
        // 1. Specify the corresponding *Command struct
        // 2. Register this *Command struct in the CScriptCommandRegistry::initialize()
        // 3. In case of AltName, see what was done for the existing *Command structs
        // 4. Add help info to the CONSOLE_CMD_INFO array (DlgConsole.cpp)
        eCmdResult Do(const tstr& params);
        eCmdResult DoCd(const tstr& params);
        eCmdResult DoCls(const tstr& params);
        eCmdResult DoClipSetText(const tstr& params);
        eCmdResult DoConColour(const tstr& params);
        eCmdResult DoConFilter(const tstr& params);
        eCmdResult DoConLoadFrom(const tstr& params);
        eCmdResult DoConSaveTo(const tstr& params);
        eCmdResult DoDir(const tstr& params);
        eCmdResult DoEcho(const tstr& params);
        eCmdResult DoElse(const tstr& params);
        eCmdResult DoEndIf(const tstr& params);
        eCmdResult DoEnvSet(const tstr& params);
        eCmdResult DoEnvUnset(const tstr& params);
        eCmdResult DoGoTo(const tstr& params);
        eCmdResult DoIf(const tstr& params);
        eCmdResult DoInputBox(const tstr& params);
        eCmdResult DoLabel(const tstr& params);
        eCmdResult DoNpeCmdAlias(const tstr& params);
        eCmdResult DoNpeConsole(const tstr& params);
        eCmdResult DoNpeDebugLog(const tstr& params);
        eCmdResult DoNpeNoEmptyVars(const tstr& params);
        eCmdResult DoNpeQueue(const tstr& params);
        eCmdResult DoNppClose(const tstr& params);
        eCmdResult DoNppConsole(const tstr& params);
        eCmdResult DoNppExec(const tstr& params);
        eCmdResult DoNppMenuCommand(const tstr& params);
        eCmdResult DoNppOpen(const tstr& params);
        eCmdResult DoNppRun(const tstr& params);
        eCmdResult DoNppSave(const tstr& params);
        eCmdResult DoNppSaveAs(const tstr& params);
        eCmdResult DoNppSaveAll(const tstr& params);
        eCmdResult DoNppSendMsg(const tstr& params);
        eCmdResult DoNppSendMsgEx(const tstr& params);
        eCmdResult DoNppSetFocus(const tstr& params);
        eCmdResult DoNppSwitch(const tstr& params);
        eCmdResult DoProcSignal(const tstr& params);
        eCmdResult DoSleep(const tstr& params);
        eCmdResult DoSciFind(const tstr& params);
        eCmdResult DoSciReplace(const tstr& params);
        eCmdResult DoSciSendMsg(const tstr& params);
        eCmdResult DoSelLoadFrom(const tstr& params);
        eCmdResult DoSelSaveTo(const tstr& params);
        eCmdResult DoSelSetText(const tstr& params);
        eCmdResult DoSelSetTextEx(const tstr& params);
        eCmdResult DoSet(const tstr& params);
        eCmdResult DoTextLoadFrom(const tstr& params);
        eCmdResult DoTextSaveTo(const tstr& params);
        eCmdResult DoUnset(const tstr& params);

        struct DoCommand
        {
            static const TCHAR* const Name() { return _T(""); }
            static const TCHAR* const AltName() { return nullptr; }
            static eCmdType           Type() { return CMDTYPE_UNKNOWN; }
            static eCmdResult         Exec(CScriptEngine* pEngine, const tstr& params) { return pEngine->Do(params); }
        };

        struct DoCdCommand
        {
            static const TCHAR* const Name() { return _T("CD"); }
            static const TCHAR* const AltName() { return nullptr; }
            static eCmdType           Type() { return CMDTYPE_CD; }
            static eCmdResult         Exec(CScriptEngine* pEngine, const tstr& params) { return pEngine->DoCd(params); }
        };

        struct DoClsCommand
        {
            static const TCHAR* const Name() { return _T("CLS"); }
            static const TCHAR* const AltName() { return nullptr; }
            static eCmdType           Type() { return CMDTYPE_CLS; }
            static eCmdResult         Exec(CScriptEngine* pEngine, const tstr& params) { return pEngine->DoCls(params); }
        };

        struct DoClipSetTextCommand
        {
            static const TCHAR* const Name() { return _T("CLIP_SETTEXT"); }
            static const TCHAR* const AltName() { return nullptr; }
            static eCmdType           Type() { return CMDTYPE_CLIPSETTEXT; }
            static eCmdResult         Exec(CScriptEngine* pEngine, const tstr& params) { return pEngine->DoClipSetText(params); }
        };

        struct DoConColourCommand
        {
            static const TCHAR* const Name() { return _T("CON_COLOUR"); }
            static const TCHAR* const AltName() { return _T("CON_COLOR"); }
            static eCmdType           Type() { return CMDTYPE_CONCOLOUR; }
            static eCmdResult         Exec(CScriptEngine* pEngine, const tstr& params) { return pEngine->DoConColour(params); }
        };

        struct DoConFilterCommand
        {
            static const TCHAR* const Name() { return _T("CON_FILTER"); }
            static const TCHAR* const AltName() { return nullptr; }
            static eCmdType           Type() { return CMDTYPE_CONFILTER; }
            static eCmdResult         Exec(CScriptEngine* pEngine, const tstr& params) { return pEngine->DoConFilter(params); }
        };

        struct DoConLoadFromCommand
        {
            static const TCHAR* const Name() { return _T("CON_LOADFROM"); }
            static const TCHAR* const AltName() { return _T("CON_LOAD"); }
            static eCmdType           Type() { return CMDTYPE_CONLOADFROM; }
            static eCmdResult         Exec(CScriptEngine* pEngine, const tstr& params) { return pEngine->DoConLoadFrom(params); }
        };

        struct DoConSaveToCommand
        {
            static const TCHAR* const Name() { return _T("CON_SAVETO"); }
            static const TCHAR* const AltName() { return _T("CON_SAVE"); }
            static eCmdType           Type() { return CMDTYPE_CONSAVETO; }
            static eCmdResult         Exec(CScriptEngine* pEngine, const tstr& params) { return pEngine->DoConSaveTo(params); }
        };

        struct DoDirCommand
        {
            static const TCHAR* const Name() { return _T("DIR"); }
            static const TCHAR* const AltName() { return nullptr; }
            static eCmdType           Type() { return CMDTYPE_DIR; }
            static eCmdResult         Exec(CScriptEngine* pEngine, const tstr& params) { return pEngine->DoDir(params); }
        };

        struct DoEchoCommand
        {
            static const TCHAR* const Name() { return _T("ECHO"); }
            static const TCHAR* const AltName() { return nullptr; }
            static eCmdType           Type() { return CMDTYPE_ECHO; }
            static eCmdResult         Exec(CScriptEngine* pEngine, const tstr& params) { return pEngine->DoEcho(params); }
        };

        struct DoElseCommand
        {
            static const TCHAR* const Name() { return _T("ELSE"); }
            static const TCHAR* const AltName() { return nullptr; }
            static eCmdType           Type() { return CMDTYPE_ELSE; }
            static eCmdResult         Exec(CScriptEngine* pEngine, const tstr& params) { return pEngine->DoElse(params); }
        };

        struct DoEndIfCommand
        {
            static const TCHAR* const Name() { return _T("ENDIF"); }
            static const TCHAR* const AltName() { return nullptr; }
            static eCmdType           Type() { return CMDTYPE_ENDIF; }
            static eCmdResult         Exec(CScriptEngine* pEngine, const tstr& params) { return pEngine->DoEndIf(params); }
        };

        struct DoEnvSetCommand
        {
            static const TCHAR* const Name() { return _T("ENV_SET"); }
            static const TCHAR* const AltName() { return _T("SET_ENV"); }
            static eCmdType           Type() { return CMDTYPE_ENVSET; }
            static eCmdResult         Exec(CScriptEngine* pEngine, const tstr& params) { return pEngine->DoEnvSet(params); }
        };

        struct DoEnvUnsetCommand
        {
            static const TCHAR* const Name() { return _T("ENV_UNSET"); }
            static const TCHAR* const AltName() { return _T("UNSET_ENV"); }
            static eCmdType           Type() { return CMDTYPE_ENVUNSET; }
            static eCmdResult         Exec(CScriptEngine* pEngine, const tstr& params) { return pEngine->DoEnvUnset(params); }
        };

        struct DoGoToCommand
        {
            static const TCHAR* const Name() { return _T("GOTO"); }
            static const TCHAR* const AltName() { return nullptr; }
            static eCmdType           Type() { return CMDTYPE_GOTO; }
            static eCmdResult         Exec(CScriptEngine* pEngine, const tstr& params) { return pEngine->DoGoTo(params); }
        };

        struct DoIfCommand
        {
            static const TCHAR* const Name() { return _T("IF"); }
            static const TCHAR* const AltName() { return nullptr; }
            static eCmdType           Type() { return CMDTYPE_IF; }
            static eCmdResult         Exec(CScriptEngine* pEngine, const tstr& params) { return pEngine->DoIf(params); }
        };

        struct DoInputBoxCommand
        {
            static const TCHAR* const Name() { return _T("INPUTBOX"); }
            static const TCHAR* const AltName() { return nullptr; }
            static eCmdType           Type() { return CMDTYPE_INPUTBOX; }
            static eCmdResult         Exec(CScriptEngine* pEngine, const tstr& params) { return pEngine->DoInputBox(params); }
        };

        struct DoLabelCommand
        {
            static const TCHAR* const Name() { return _T("LABEL"); }
            static const TCHAR* const AltName() { return nullptr; }
            static eCmdType           Type() { return CMDTYPE_LABEL; }
            static eCmdResult         Exec(CScriptEngine* pEngine, const tstr& params) { return pEngine->DoLabel(params); }
        };

        struct DoNpeCmdAliasCommand
        {
            static const TCHAR* const Name() { return _T("NPE_CMDALIAS"); }
            static const TCHAR* const AltName() { return nullptr; }
            static eCmdType           Type() { return CMDTYPE_NPECMDALIAS; }
            static eCmdResult         Exec(CScriptEngine* pEngine, const tstr& params) { return pEngine->DoNpeCmdAlias(params); }
        };

        struct DoNpeConsoleCommand
        {
            static const TCHAR* const Name() { return _T("NPE_CONSOLE"); }
            static const TCHAR* const AltName() { return nullptr; }
            static eCmdType           Type() { return CMDTYPE_NPECONSOLE; }
            static eCmdResult         Exec(CScriptEngine* pEngine, const tstr& params) { return pEngine->DoNpeConsole(params); }
        };

        struct DoNpeDebugLogCommand
        {
            static const TCHAR* const Name() { return _T("NPE_DEBUGLOG"); }
            static const TCHAR* const AltName() { return _T("NPE_DEBUG"); }
            static eCmdType           Type() { return CMDTYPE_NPEDEBUGLOG; }
            static eCmdResult         Exec(CScriptEngine* pEngine, const tstr& params) { return pEngine->DoNpeDebugLog(params); }
        };

        struct DoNpeNoEmptyVarsCommand
        {
            static const TCHAR* const Name() { return _T("NPE_NOEMPTYVARS"); }
            static const TCHAR* const AltName() { return nullptr; }
            static eCmdType           Type() { return CMDTYPE_NPENOEMPTYVARS; }
            static eCmdResult         Exec(CScriptEngine* pEngine, const tstr& params) { return pEngine->DoNpeNoEmptyVars(params); }
        };

        struct DoNpeQueueCommand
        {
            static const TCHAR* const Name() { return _T("NPE_QUEUE"); }
            static const TCHAR* const AltName() { return nullptr; }
            static eCmdType           Type() { return CMDTYPE_NPEQUEUE; }
            static eCmdResult         Exec(CScriptEngine* pEngine, const tstr& params) { return pEngine->DoNpeQueue(params); }
        };

        struct DoNppCloseCommand
        {
            static const TCHAR* const Name() { return _T("NPP_CLOSE"); }
            static const TCHAR* const AltName() { return nullptr; }
            static eCmdType           Type() { return CMDTYPE_NPPCLOSE; }
            static eCmdResult         Exec(CScriptEngine* pEngine, const tstr& params) { return pEngine->DoNppClose(params); }
        };

        struct DoNppConsoleCommand
        {
            static const TCHAR* const Name() { return _T("NPP_CONSOLE"); }
            static const TCHAR* const AltName() { return nullptr; }
            static eCmdType           Type() { return CMDTYPE_NPPCONSOLE; }
            static eCmdResult         Exec(CScriptEngine* pEngine, const tstr& params) { return pEngine->DoNppConsole(params); }
        };

        struct DoNppExecCommand
        {
            static const TCHAR* const Name() { return _T("NPP_EXEC"); }
            static const TCHAR* const AltName() { return nullptr; }
            static eCmdType           Type() { return CMDTYPE_NPPEXEC; }
            static eCmdResult         Exec(CScriptEngine* pEngine, const tstr& params) { return pEngine->DoNppExec(params); }
        };

        struct DoNppMenuCommandCommand
        {
            static const TCHAR* const Name() { return _T("NPP_MENUCOMMAND"); }
            static const TCHAR* const AltName() { return nullptr; }
            static eCmdType           Type() { return CMDTYPE_NPPMENUCOMMAND; }
            static eCmdResult         Exec(CScriptEngine* pEngine, const tstr& params) { return pEngine->DoNppMenuCommand(params); }
        };

        struct DoNppOpenCommand
        {
            static const TCHAR* const Name() { return _T("NPP_OPEN"); }
            static const TCHAR* const AltName() { return nullptr; }
            static eCmdType           Type() { return CMDTYPE_NPPOPEN; }
            static eCmdResult         Exec(CScriptEngine* pEngine, const tstr& params) { return pEngine->DoNppOpen(params); }
        };

        struct DoNppRunCommand
        {
            static const TCHAR* const Name() { return _T("NPP_RUN"); }
            static const TCHAR* const AltName() { return nullptr; }
            static eCmdType           Type() { return CMDTYPE_NPPRUN; }
            static eCmdResult         Exec(CScriptEngine* pEngine, const tstr& params) { return pEngine->DoNppRun(params); }
        };

        struct DoNppSaveCommand
        {
            static const TCHAR* const Name() { return _T("NPP_SAVE"); }
            static const TCHAR* const AltName() { return nullptr; }
            static eCmdType           Type() { return CMDTYPE_NPPSAVE; }
            static eCmdResult         Exec(CScriptEngine* pEngine, const tstr& params) { return pEngine->DoNppSave(params); }
        };

        struct DoNppSaveAsCommand
        {
            static const TCHAR* const Name() { return _T("NPP_SAVEAS"); }
            static const TCHAR* const AltName() { return nullptr; }
            static eCmdType           Type() { return CMDTYPE_NPPSAVEAS; }
            static eCmdResult         Exec(CScriptEngine* pEngine, const tstr& params) { return pEngine->DoNppSaveAs(params); }
        };

        struct DoNppSaveAllCommand
        {
            static const TCHAR* const Name() { return _T("NPP_SAVEALL"); }
            static const TCHAR* const AltName() { return nullptr; }
            static eCmdType           Type() { return CMDTYPE_NPPSAVEALL; }
            static eCmdResult         Exec(CScriptEngine* pEngine, const tstr& params) { return pEngine->DoNppSaveAll(params); }
        };

        struct DoNppSendMsgCommand
        {
            static const TCHAR* const Name() { return _T("NPP_SENDMSG"); }
            static const TCHAR* const AltName() { return nullptr; }
            static eCmdType           Type() { return CMDTYPE_NPPSENDMSG; }
            static eCmdResult         Exec(CScriptEngine* pEngine, const tstr& params) { return pEngine->DoNppSendMsg(params); }
        };

        struct DoNppSendMsgExCommand
        {
            static const TCHAR* const Name() { return _T("NPP_SENDMSGEX"); }
            static const TCHAR* const AltName() { return nullptr; }
            static eCmdType           Type() { return CMDTYPE_NPPSENDMSGEX; }
            static eCmdResult         Exec(CScriptEngine* pEngine, const tstr& params) { return pEngine->DoNppSendMsgEx(params); }
        };

        struct DoNppSetFocusCommand
        {
            static const TCHAR* const Name() { return _T("NPP_SETFOCUS"); }
            static const TCHAR* const AltName() { return nullptr; }
            static eCmdType           Type() { return CMDTYPE_NPPSETFOCUS; }
            static eCmdResult         Exec(CScriptEngine* pEngine, const tstr& params) { return pEngine->DoNppSetFocus(params); }
        };

        struct DoNppSwitchCommand
        {
            static const TCHAR* const Name() { return _T("NPP_SWITCH"); }
            static const TCHAR* const AltName() { return nullptr; }
            static eCmdType           Type() { return CMDTYPE_NPPSWITCH; }
            static eCmdResult         Exec(CScriptEngine* pEngine, const tstr& params) { return pEngine->DoNppSwitch(params); }
        };

        struct DoProcSignalCommand
        {
            static const TCHAR* const Name() { return _T("PROC_SIGNAL"); }
            static const TCHAR* const AltName() { return nullptr; }
            static eCmdType           Type() { return CMDTYPE_PROCSIGNAL; }
            static eCmdResult         Exec(CScriptEngine* pEngine, const tstr& params) { return pEngine->DoProcSignal(params); }
        };

        struct DoSleepCommand
        {
            static const TCHAR* const Name() { return _T("SLEEP"); }
            static const TCHAR* const AltName() { return nullptr; }
            static eCmdType           Type() { return CMDTYPE_SLEEP; }
            static eCmdResult         Exec(CScriptEngine* pEngine, const tstr& params) { return pEngine->DoSleep(params); }
        };

        struct DoSciFindCommand
        {
            static const TCHAR* const Name() { return _T("SCI_FIND"); }
            static const TCHAR* const AltName() { return nullptr; }
            static eCmdType           Type() { return CMDTYPE_SCIFIND; }
            static eCmdResult         Exec(CScriptEngine* pEngine, const tstr& params) { return pEngine->DoSciFind(params); }
        };

        struct DoSciReplaceCommand
        {
            static const TCHAR* const Name() { return _T("SCI_REPLACE"); }
            static const TCHAR* const AltName() { return nullptr; }
            static eCmdType           Type() { return CMDTYPE_SCIREPLACE; }
            static eCmdResult         Exec(CScriptEngine* pEngine, const tstr& params) { return pEngine->DoSciReplace(params); }
        };

        struct DoSciSendMsgCommand
        {
            static const TCHAR* const Name() { return _T("SCI_SENDMSG"); }
            static const TCHAR* const AltName() { return nullptr; }
            static eCmdType           Type() { return CMDTYPE_SCISENDMSG; }
            static eCmdResult         Exec(CScriptEngine* pEngine, const tstr& params) { return pEngine->DoSciSendMsg(params); }
        };

        struct DoSelLoadFromCommand
        {
            static const TCHAR* const Name() { return _T("SEL_LOADFROM"); }
            static const TCHAR* const AltName() { return _T("SEL_LOAD"); }
            static eCmdType           Type() { return CMDTYPE_SELLOADFROM; }
            static eCmdResult         Exec(CScriptEngine* pEngine, const tstr& params) { return pEngine->DoSelLoadFrom(params); }
        };

        struct DoSelSaveToCommand
        {
            static const TCHAR* const Name() { return _T("SEL_SAVETO"); }
            static const TCHAR* const AltName() { return _T("SEL_SAVE"); }
            static eCmdType           Type() { return CMDTYPE_SELSAVETO; }
            static eCmdResult         Exec(CScriptEngine* pEngine, const tstr& params) { return pEngine->DoSelSaveTo(params); }
        };

        struct DoSelSetTextCommand
        {
            static const TCHAR* const Name() { return _T("SEL_SETTEXT"); }
            static const TCHAR* const AltName() { return nullptr; }
            static eCmdType           Type() { return CMDTYPE_SELSETTEXT; }
            static eCmdResult         Exec(CScriptEngine* pEngine, const tstr& params) { return pEngine->DoSelSetText(params); }
        };

        struct DoSelSetTextExCommand
        {
            static const TCHAR* const Name() { return _T("SEL_SETTEXT+"); }
            static const TCHAR* const AltName() { return nullptr; }
            static eCmdType           Type() { return CMDTYPE_SELSETTEXTEX; }
            static eCmdResult         Exec(CScriptEngine* pEngine, const tstr& params) { return pEngine->DoSelSetTextEx(params); }
        };

        struct DoSetCommand
        {
            static const TCHAR* const Name() { return _T("SET"); }
            static const TCHAR* const AltName() { return nullptr; }
            static eCmdType           Type() { return CMDTYPE_SET; }
            static eCmdResult         Exec(CScriptEngine* pEngine, const tstr& params) { return pEngine->DoSet(params); }
        };

        struct DoTextLoadFromCommand
        {
            static const TCHAR* const Name() { return _T("TEXT_LOADFROM"); }
            static const TCHAR* const AltName() { return _T("TEXT_LOAD"); }
            static eCmdType           Type() { return CMDTYPE_TEXTLOADFROM; }
            static eCmdResult         Exec(CScriptEngine* pEngine, const tstr& params) { return pEngine->DoTextLoadFrom(params); }
        };

        struct DoTextSaveToCommand
        {
            static const TCHAR* const Name() { return _T("TEXT_SAVETO"); }
            static const TCHAR* const AltName() { return _T("TEXT_SAVE"); }
            static eCmdType           Type() { return CMDTYPE_TEXTSAVETO; }
            static eCmdResult         Exec(CScriptEngine* pEngine, const tstr& params) { return pEngine->DoTextSaveTo(params); }
        };

        struct DoUnsetCommand
        {
            static const TCHAR* const Name() { return _T("UNSET"); }
            static const TCHAR* const AltName() { return nullptr; }
            static eCmdType           Type() { return CMDTYPE_UNSET; }
            static eCmdResult         Exec(CScriptEngine* pEngine, const tstr& params) { return pEngine->DoUnset(params); }
        };

        class CScriptCommandRegistry
        {
            public:
                CScriptCommandRegistry()
                {
                    CCriticalSectionLockGuard lock(m_csConstruct); // just in case

                    if ( m_CommandTypeByName.empty() )
                    {
                        // initialize() was not called

                      #ifdef _DEBUG
                        for ( auto& p : m_CommandExecFunc )
                        {
                            p = nullptr;
                        }
                        for ( auto& n : m_CommandNameByType )
                        {
                            n = nullptr;
                        }
                      #endif

                        initialize();

                      #ifdef _DEBUG
                        // there must be no nullptr items after initialize() !!!
                        for ( auto& p : m_CommandExecFunc )
                        {
                            assert( p != nullptr );
                        }
                        for ( auto& n : m_CommandNameByType )
                        {
                            assert( n != nullptr );
                        }
                      #endif
                    }
                }

                const TCHAR* const GetCmdNameByType(eCmdType cmdType) const
                {
                    return m_CommandNameByType[cmdType];
                }

                eCmdType GetCmdTypeByName(const tstr& cmdName) const
                {
                    auto itr = m_CommandTypeByName.find(cmdName);
                    return ( (itr != m_CommandTypeByName.end()) ? itr->second : CMDTYPE_UNKNOWN );
                }

                const std::list<tstr>& GetSortedCmdNames() const
                {
                    return m_SortedCommandNames;
                }

                EXECFUNC GetCmdExecFunc(eCmdType cmdType) const
                {
                    return m_CommandExecFunc[cmdType];
                }

            protected:
                template<class C> void registerCommand()
                {
                    const TCHAR* const cmdName = C::Name();
                    const TCHAR* const cmdAltName = C::AltName();
                    const eCmdType cmdType = C::Type();
                    m_CommandExecFunc[cmdType] = &C::Exec;
                    m_CommandNameByType[cmdType] = cmdName;
                    m_CommandTypeByName[cmdName] = cmdType;
                    if ( cmdAltName != nullptr )
                    {
                        m_CommandTypeByName[cmdAltName] = cmdType;
                    }
                    m_SortedCommandNames.push_back(cmdName); // does _not_ include cmdAltName!
                }

                void initialize()
                {
                    // 1. register commands
                    registerCommand<DoCommand>();
                    registerCommand<DoCdCommand>();
                    registerCommand<DoClsCommand>();
                    registerCommand<DoClipSetTextCommand>();
                    registerCommand<DoConColourCommand>();
                    registerCommand<DoConFilterCommand>();
                    registerCommand<DoConLoadFromCommand>();
                    registerCommand<DoConSaveToCommand>();
                    registerCommand<DoDirCommand>();
                    registerCommand<DoEchoCommand>();
                    registerCommand<DoElseCommand>();
                    registerCommand<DoEndIfCommand>();
                    registerCommand<DoEnvSetCommand>();
                    registerCommand<DoEnvUnsetCommand>();
                    registerCommand<DoGoToCommand>();
                    registerCommand<DoIfCommand>();
                    registerCommand<DoInputBoxCommand>();
                    registerCommand<DoLabelCommand>();
                    registerCommand<DoNpeCmdAliasCommand>();
                    registerCommand<DoNpeConsoleCommand>();
                    registerCommand<DoNpeDebugLogCommand>();
                    registerCommand<DoNpeNoEmptyVarsCommand>();
                    registerCommand<DoNpeQueueCommand>();
                    registerCommand<DoNppCloseCommand>();
                    registerCommand<DoNppConsoleCommand>();
                    registerCommand<DoNppExecCommand>();
                    registerCommand<DoNppMenuCommandCommand>();
                    registerCommand<DoNppOpenCommand>();
                    registerCommand<DoNppRunCommand>();
                    registerCommand<DoNppSaveCommand>();
                    registerCommand<DoNppSaveAsCommand>();
                    registerCommand<DoNppSaveAllCommand>();
                    registerCommand<DoNppSendMsgCommand>();
                    registerCommand<DoNppSendMsgExCommand>();
                    registerCommand<DoNppSetFocusCommand>();
                    registerCommand<DoNppSwitchCommand>();
                    registerCommand<DoProcSignalCommand>();
                    registerCommand<DoSleepCommand>();
                    registerCommand<DoSciFindCommand>();
                    registerCommand<DoSciReplaceCommand>();
                    registerCommand<DoSciSendMsgCommand>();
                    registerCommand<DoSelLoadFromCommand>();
                    registerCommand<DoSelSaveToCommand>();
                    registerCommand<DoSelSetTextCommand>();
                    registerCommand<DoSelSetTextExCommand>();
                    registerCommand<DoSetCommand>();
                    registerCommand<DoTextLoadFromCommand>();
                    registerCommand<DoTextSaveToCommand>();
                    registerCommand<DoUnsetCommand>();

                    // 2. sort names
                    m_SortedCommandNames.sort();
                }

            protected:
                CCriticalSection m_csConstruct;
                EXECFUNC m_CommandExecFunc[CMDTYPE_TOTAL_COUNT];
                const TCHAR* m_CommandNameByType[CMDTYPE_TOTAL_COUNT];
                std::map<tstr, eCmdType> m_CommandTypeByName;
                std::list<tstr> m_SortedCommandNames;
        };

    public:
        static CScriptCommandRegistry& GetCommandRegistry()
        {
            return m_CommandRegistry;
        }

        void Run(unsigned int nRunFlags = 0);

        bool IsSharingLocalVars() const { return ((m_nRunFlags & rfShareLocalVars) != 0); }
        bool IsCollateral() const { return ((m_nRunFlags & rfCollateralScript) != 0); }
        bool IsExternal() const { return ((m_nRunFlags & rfExternal) != 0); }
        bool IsClosingConsole() const { return m_isClosingConsole; } // "npp_console off" is in progress

        void DoNotShareLocalVars()
        {
            m_nRunFlags &= ~rfShareLocalVars;
        }

        bool ContinueExecution() const;

        eCmdType GetLastCmdType() const  { return m_nCmdType; }
        //int  GetLastCmdResult() const  { return m_nLastCmdResult; }
        const tstr& GetLastLoggedCmd() const  { return m_sLoggedCmd; }
        const tstr& GetLastCmdParams() const  { return m_sCmdParams; }

        static eNppExecCmdPrefix checkNppExecCmdPrefix(CNppExec* pNppExec, tstr& Cmd, bool bRemovePrefix = true);
        static eCmdType getCmdType(CNppExec* pNppExec, tstr& Cmd, unsigned int nFlags = ctfUseLogging);
        static int      getOnOffParam(const tstr& param);
        static bool     isCommentOrEmpty(CNppExec* pNppExec, tstr& Cmd);
        static bool     isSkippingThisCommandDueToIfState(eCmdType cmdType, eIfState ifState);
        static eCmdType modifyCommandLine(CScriptEngine* pScriptEngine, tstr& Cmd, eIfState ifState);

        CScriptEngine(CNppExec* pNppExec, const CListT<tstr>& CmdList, const tstr& id);
        virtual ~CScriptEngine();

        CNppExec* GetNppExec() const { return m_pNppExec; }

        const TCHAR* GetInstanceStr() const;

        const tstr& GetId() const { return m_id; }

        DWORD GetThreadId() const { return m_dwThreadId; }

        CListT<tstr>& GetCmdList() { return m_CmdList; }
        const std::shared_ptr<CScriptEngine> GetParentScriptEngine() const { return m_pParentScriptEngine; }
        std::shared_ptr<CScriptEngine> GetParentScriptEngine() { return m_pParentScriptEngine; }
        const std::shared_ptr<CScriptEngine> GetChildScriptEngine() const { return m_pChildScriptEngine; }
        std::shared_ptr<CScriptEngine> GetChildScriptEngine() { return m_pChildScriptEngine; }
        bool IsParentOf(const std::shared_ptr<CScriptEngine> pScriptEngine) const;
        bool IsChildOf(const std::shared_ptr<CScriptEngine> pScriptEngine) const;

        void SetParentScriptEngine(std::shared_ptr<CScriptEngine> pParentEngine)
        {
            m_pParentScriptEngine = pParentEngine;
        }

        void SetChildScriptEngine(std::shared_ptr<CScriptEngine> pChildEngine)
        {
            m_pChildScriptEngine = pChildEngine;
        }

        virtual bool IsDone() const { return (m_eventRunIsDone.Wait(0) == WAIT_OBJECT_0); }
        virtual bool IsAborted() const { return (m_eventAbortTheScript.Wait(0) == WAIT_OBJECT_0); }
        virtual bool IsChildProcessRunning() const;
        virtual std::shared_ptr<CChildProcess> GetRunningChildProcess();

        virtual bool GetTriedExitCmd() const { return m_bTriedExitCmd; }
        virtual void SetTriedExitCmd(bool bTriedExitCmd) { m_bTriedExitCmd = bTriedExitCmd; }

        virtual void ScriptError(eErrorType type, const TCHAR* cszErrorMessage);
        void UndoAbort(const TCHAR* cszMessage);

        void ChildProcessMustBreakAll();
        bool WaitUntilDone(DWORD dwTimeoutMs) const;

    public:
        typedef std::map< tstr, CListItemT<tstr>* > tLabels;
        typedef CNppExecMacroVars::tMacroVars tMacroVars;

        typedef struct sCmdRange {
            CListItemT<tstr>* pBegin; // points to first cmd
            CListItemT<tstr>* pEnd;   // points _after_ last cmd
        } tCmdRange;

        class ScriptContext {
            public:
                tstr         ScriptName;
                tCmdRange    CmdRange;
                tLabels      Labels;
                tMacroVars   LocalMacroVars; // use with GetMacroVars().GetCsUserMacroVars()
                bool         IsNppExeced;
            
            protected:
                CBufT<eIfState> IfState;

                static void strAppendIfState(tstr& S, eIfState ifState)
                {
                    const TCHAR* cszState = _T("???");
                    switch ( ifState )
                    {
                        case IF_NONE:
                            cszState = _T("IF_NONE");
                            break;
                        case IF_EXECUTING:
                            cszState = _T("IF_EXECUTING");
                            break;
                        case IF_WANT_ELSE:
                            cszState = _T("IF_WANT_ELSE");
                            break;
                        case IF_WANT_ENDIF:
                            cszState = _T("IF_WANT_ENDIF");
                            break;
                        case IF_MAYBE_ELSE:
                            cszState = _T("IF_MAYBE_ELSE");
                            break;
                        case IF_WANT_SILENT_ENDIF:
                            cszState = _T("IF_WANT_SILENT_ENDIF");
                            break;
                    }

                    if ( !S.IsEmpty() )
                        S += _T(", ");

                    S += cszState;
                }
                
                void logIfState(const TCHAR* cszFuncName)
                {
                    tstr S;
                    const int nIfStates = IfState.size();
                    S.Reserve(20 * nIfStates);
                    for ( int i = 0; i < nIfStates; ++i )
                    {
                        strAppendIfState(S, IfState[i]);
                    }

                    Runtime::GetLogger().AddEx_WithoutOutput( _T("; %s() : IfState[%u] = { %s }"), cszFuncName, IfState.size(), S.c_str() );
                }

            public:
                ScriptContext()
                {
                    CmdRange.pBegin = 0;
                    CmdRange.pEnd = 0;
                    IsNppExeced = false;
                }

                const int GetIfDepth() const
                {
                    return IfState.size();
                }

                const eIfState GetIfState() const
                {
                    const int nSize = IfState.size();
                    return ((nSize != 0) ? IfState[nSize - 1] : IF_NONE);
                }

                void SetIfState(eIfState ifState)
                {
                    IfState[IfState.size() - 1] = ifState;

                    logIfState( _T("SetIfState") );
                }

                void PushIfState(eIfState ifState)
                {
                    if ( GetIfState() == IF_MAYBE_ELSE )
                    {
                        SetIfState(IF_NONE); // remove IF_MAYBE_ELSE because of nested IF
                    }
                    IfState.Append(ifState);

                    logIfState( _T("PushIfState") );
                }

                void PopIfState()
                {
                    IfState.SetSize(IfState.size() - 1);

                    logIfState( _T("PopIfState") );
                }
        };

        typedef struct tExecState {
            // these 4 variables are needed to prevent infinite loops
            int nExecCounter;
            int nExecMaxCount;
            int nGoToCounter;
            int nGoToMaxCount;
            // script name, cmd range, labels and so on
            CListT<ScriptContext> ScriptContextList;
            // pointer to current script line in m_CmdList
            CListItemT<tstr>* pScriptLineCurrent;
            // set this value to jump to a specific script line:
            CListItemT<tstr>* pScriptLineNext;
            CListItemT<tstr>* pScriptLineNextBackupCopy;
            // child console process
            std::shared_ptr<CChildProcess> pChildProcess;

            void SetScriptLineNext(CListItemT<tstr>* p)
            {
                pScriptLineNext = p;
                pScriptLineNextBackupCopy = p;
            }
            
            CListItemT<ScriptContext>* GetScriptContextItemPtr(const tstr& ScriptName)
            {
                CListItemT<CScriptEngine::ScriptContext>* p = ScriptContextList.GetFirst();
                while ( p && (p->GetItem().ScriptName != ScriptName) )
                {
                    p = p->GetNext();
                }
                return p;
            }

            ScriptContext& GetCurrentScriptContext()
            {
                return ScriptContextList.GetLast()->GetItem();
            }

            std::shared_ptr<CChildProcess> GetRunningChildProcess()
            {
                return pChildProcess;
            }

            bool IsChildProcessRunning() const
            {
                return (bool)(pChildProcess);
            }
        } ExecState;

        const ExecState& GetExecState() const { return m_execState; }
        ExecState& GetExecState() { return m_execState; }

    protected:
        enum eReportFlags {
            fMessageToConsole = 0x01,
            fReportEmptyParam = 0x02,
            fFailIfEmptyParam = 0x10
        };

        static CScriptCommandRegistry m_CommandRegistry; // one and only, for all script engines

        std::shared_ptr<CScriptEngine> m_pParentScriptEngine;
        std::shared_ptr<CScriptEngine> m_pChildScriptEngine;
        CNppExec*      m_pNppExec;
        CListT<tstr>   m_CmdList; // let the script have its own copy of CmdList
        tstr           m_strInstance;
        tstr           m_id;
        ExecState      m_execState;
        eCmdType       m_nCmdType;       // is not updated for empty command type
        //int          m_nLastCmdResult; // is not updated for empty command type
        tstr           m_sCmdParams;
        tstr           m_sLoggedCmd; // last cmd as it's present in the log
        unsigned int   m_nRunFlags;
        DWORD          m_dwThreadId;
        bool           m_bTriedExitCmd;
        bool           m_isClosingConsole;
        CEvent         m_eventRunIsDone;
        CEvent         m_eventAbortTheScript;

        void errorCmdNotEnoughParams(const TCHAR* cszCmd, const TCHAR* cszErrorMessage);
        void errorCmdNoParam(const TCHAR* cszCmd);
        void messageConsole(const TCHAR* cszCmd, const TCHAR* cszParams);
        bool reportCmdAndParams(const TCHAR* cszCmd, const tstr& params, unsigned int uFlags);
        void updateFocus();

        eCmdResult doSendMsg(const tstr& params, int cmdType);
        eCmdResult doSciFindReplace(const tstr& params, eCmdType cmdType);
        eCmdResult doIf(const tstr& params, bool isElseIf);
        eCmdResult doTextLoad(const tstr& params, eCmdType cmdType);
        eCmdResult doTextSave(const tstr& params, eCmdType cmdType);

};

class CScriptEngineRunner
{
    public:
        CScriptEngineRunner(CScriptEngine* pScriptEngine, unsigned int nRunFlags)
          : m_pScriptEngine(pScriptEngine)
          , m_nRunFlags(nRunFlags)
        {
        }

        CScriptEngine* GetScriptEngine() const { return m_pScriptEngine; }
        unsigned int   GetRunFlags() const { return m_nRunFlags; }

    protected:
        CScriptEngine* m_pScriptEngine;
        unsigned int m_nRunFlags;
};

extern const TCHAR MACRO_FILE_FULLPATH[];
extern const TCHAR MACRO_FILE_DIRPATH[];
extern const TCHAR MACRO_FILE_FULLNAME[];
extern const TCHAR MACRO_FILE_NAMEONLY[];
extern const TCHAR MACRO_FILE_EXTONLY[];
extern const TCHAR MACRO_NPP_DIRECTORY[];
extern const TCHAR MACRO_CURRENT_WORD[];
extern const TCHAR MACRO_CURRENT_LINE[];
extern const TCHAR MACRO_CURRENT_COLUMN[];
extern const TCHAR MACRO_DOCNUMBER[];
extern const TCHAR MACRO_SYSVAR[];
extern const TCHAR MACRO_LEFT_VIEW_FILE[];
extern const TCHAR MACRO_RIGHT_VIEW_FILE[];
extern const TCHAR MACRO_PLUGINS_CONFIG_DIR[];
extern const TCHAR MACRO_CURRENT_WORKING_DIR[];
extern const TCHAR MACRO_ARGC[];
extern const TCHAR MACRO_ARGV[];
extern const TCHAR MACRO_RARGV[];
extern const TCHAR MACRO_INPUT[];
extern const TCHAR MACRO_OUTPUT[];
extern const TCHAR MACRO_OUTPUT1[];
extern const TCHAR MACRO_OUTPUTL[];
extern const TCHAR MACRO_EXITCODE[];
extern const TCHAR MACRO_PID[];
extern const TCHAR MACRO_MSG_RESULT[];
extern const TCHAR MACRO_MSG_WPARAM[];
extern const TCHAR MACRO_MSG_LPARAM[];
extern const TCHAR MACRO_EXIT_CMD[];
extern const TCHAR MACRO_EXIT_CMD_SILENT[];
extern const TCHAR MACRO_LAST_CMD_RESULT[];
extern const TCHAR MACRO_CLIPBOARD_TEXT[];

//---------------------------------------------------------------------------
#endif
