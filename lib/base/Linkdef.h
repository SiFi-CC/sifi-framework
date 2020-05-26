// @(#)lib/base:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#ifdef __CLING__

#pragma link C++ class SLocator+;
#pragma link C++ class SCategory+;
#pragma link C++ class SCategoryHeader+;
#pragma link C++ class SCategoryIndex+;
#pragma link C++ class SDetector+;
#pragma link C++ class STask+;
#pragma link C++ class STaskManager+;

#pragma link C++ class SiFi;

// datasources
#pragma link C++ class SDDSignal+;
#pragma link C++ class SDDSamples+;

// util
#pragma link C++ class SRootFileHeader+;

// event
#pragma link C++ class SEvent+;
#endif
