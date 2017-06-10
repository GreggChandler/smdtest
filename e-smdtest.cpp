/*
 *	Copyright (C) 2017-2017, by Enthusiastic Software
 *	All Rights Reserved
 *
 *	e-smdtest.cpp
 *
 */

#define	ESXMAXSAMPLES	300	/* Trials			*/
#define	ESXHISTOGRAM	1	/* Non-Zero for Histogram	*/
#define	ESXSTALLS	0	/* Non-Zero for Stalls		*/
#define	ESXLOOPCOUNT	0	/* Non-Zero for Loop Count	*/
#define	ESXRESULTS	0	/* Non-Zero for Results Array	*/
#define	ESXTWOREAD	0	/* Non-Zero for Two Reads	*/

#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<math.h>
#include	<unistd.h>

#include	"esxtypes.h"

#include	"esxecore.h"

#include	"bakery.h"
#include	"barrier.h"
#include	"epilog.h"
#include	"esxfatal.h"
#include	"esxxm.h"

#include	"hist.h"
#include	"stats.h"

void UserInt()	__attribute__ ((interrupt()));

class eSMDTest : public ESXAppCore {

  private:
    bool		bDebug;
    uint		uHibernateHost;
    volatile uint *	pIntCore;
    volatile uint *	pIntCoreOther;
    volatile uint *	pIntExt;
    uint		uOtherCoreRow;
    uint		uOtherCoreCol;
#if	ESXRESULTS
    bool		bResults;
#endif
    bool		bStalls;
    Stats		statsCore;
    uint		uTestCoreFirst;
    uint		uTestCoreLast;
    bool		bTestSameCoreMemory;
    bool		bTestOtherCoreMemory;

#if	ESXRESULTS
    uint16_t		uResults[ESXMAXSAMPLES];
#endif
#if	ESXSTALLS
    uint16_t		uStalls[ESXMAXSAMPLES];
#endif
#if	ESXLOOPCOUNT
    uint16_t		uLoopCounts[ESXMAXSAMPLES];
#endif
#if	ESXHISTOGRAM
    Hist16::HistElem	HistData[500];
    Hist16		Hist;
#endif

  public:
    eSMDTest()
    : Hist(500,HistData) {
      bDebug = false;
      uHibernateHost = 90;
      uOtherCoreRow = 0;
      uOtherCoreCol = 0;
#if	ESXRESULTS
#if	ESXSTALLS
      bResults = true;
#else
      bResults = false;
#endif
#endif
      bStalls = false;
      uTestCoreFirst = 0;
#if	0==1
      uTestCoreLast = epiConfigCore.CoreCount()-1;
#else
      uTestCoreLast = 0;
#endif
      bTestSameCoreMemory = false;
      bTestOtherCoreMemory = false;
      };

  public:
    void InitInParallel(uint uThread) XMTEXT();
    void ProcessArgs(uint uThread,int iArgCount,LPCTSTR pArgValues[]) XMTEXT();
    int  Exec(uint uThread);
    void TermInParallel(uint uThread) XMTEXT();

  private:
    //void Idle();
    void WakeUp(uint uThread) XMTEXT();
    void Help() XMTEXT();

  };

eSMDTest			epiAppCore;
ESXAppCore *			pAppCore = &epiAppCore;

uint				uIntCore;

void eSMDTest::InitInParallel(uint uThread)
  {
  ESXAppCore::InitInParallel(uThread);
  pIntExt = (uint*)xmAllocN(__FILE__,sizeof(int));
  pIntCore = &uIntCore;
  }

void eSMDTest::TermInParallel(uint uThread)
  {
  xmFreeN(__FILE__,sizeof(int));
  ESXAppCore::TermInParallel(uThread);
  }

void eSMDTest::ProcessArgs(uint uThread,int iArgCount,LPCTSTR pArgValues[])
  {
  int		i;
  LPCTSTR	pArg;
  int		iArgCountSave;
  LPCTSTR *	pArgValuesSave;
  //
  iArgCountSave = iArgCount;
  pArgValuesSave = pArgValues;
  //
  iArgCount--;
  pArgValues++;
  while(iArgCount-->0)
    {
    pArg = *pArgValues++;
#if	0==1
    epiPrintF("[%2d]%s %s %d\n",uThread,pArg,*pArgValues,iArgCount);
#endif
    if     (strcmp(pArg,"-debug")==0)
	  bDebug = true;
    else if(strcmp(pArg,"-testcore")==0 && iArgCount>0)
	  {
	  iArgCount--;
	  uTestCoreFirst = atoi(*pArgValues++);
	  uTestCoreLast = uTestCoreFirst;
	  }
    else if(strcmp(pArg,"-hibernate-host")==0 && iArgCount>0)
	  {
	  iArgCount--;
	  uHibernateHost = atoi(*pArgValues++);
	  }
    else if(strcmp(pArg,"-othercore")==0 && iArgCount>1)
	  {
	  iArgCount--;
	  uOtherCoreRow = atoi(*pArgValues++);
	  iArgCount--;
	  uOtherCoreCol = atoi(*pArgValues++);
	  //
	  bTestOtherCoreMemory = true;
	  pIntCoreOther = (uint*)ESXEpiCore::Workgroup::GlobalAddress(
	    uOtherCoreRow,uOtherCoreCol,(const void *)pIntCore);
	  }
#if	ESXRESULTS
    else if(strcmp(pArg,"-results")==0)
	  bResults = true;
#endif
    else if(strcmp(pArg,"-stalls")==0)
	  bStalls = true;
    else if(strcmp(pArg,"-same-core-memory")==0)
	  bTestSameCoreMemory = true;
    else if(strcmp(pArg,"-help")==0)
	  Help();
    else
	  {
	  epiPrintF("[%d]Unsupported Argument: %s\n",uThread,pArg);
	  };
    };
  //
  if(uThread==uTestCoreFirst)
    {
    //epiPrintF("[%d]ArgCount: %d\n",uThread,iArgCountSave);
    for(i=0;i<iArgCountSave;i++)
      epiPrintF("[%d]Arg[%d]: %s\n",uThread,i,pArgValuesSave[i]);
    epiPrintF("ESXTWOREAD: %d\n",ESXTWOREAD);
    };
  }

int eSMDTest::Exec(uint uThread)
  {
  FNA("eSMDTest::Exec()");
  int		i;
  int		j;
  uint		uStart;
  uint		uStop;
  uint		uTare;
  //
#if	ESXLOOPCOUNT
  uint		uLoopCount;
#endif
  uint		uMax;
  uint		uMin;
  uint		uHistValue;
  uint		uMeasurement;
  uint		uStallStart;
  uint		uStallStop;
  uint		uTotal;
  //
  register volatile uint *	pInt;
  register uint			uTemp0;
  register uint			uValue;
  //
  ESXEpiCore::Interrupts::Attach(E_USER_INT,UserInt);
  ESXEpiCore::Interrupts::Enable(E_USER_INT,true);
  ESXEpiCore::Interrupts::GlobalEnable(true);
  //
  if     (bTestSameCoreMemory)
        pInt = pIntCore;
  else if(bTestOtherCoreMemory)
	pInt = pIntCoreOther;
  else
        pInt = pIntExt;
  //
  BarrierWait();
  if(uThread==0)
    epiHibernateHost(uHibernateHost);
  //
  for(i=uTestCoreFirst;i<=uTestCoreLast;i++)
    if(i!=uThread)
	{
	//ESXEpiCore::SleepInCore(1,5);
	  for(j=0;j<128;j++)
	    {
	    __asm volatile (" nop ");
	    __asm volatile (" nop ");
	    __asm volatile (" nop ");
	    __asm volatile (" nop ");
	    __asm volatile (" nop ");
	    __asm volatile (" nop ");
	    __asm volatile (" nop ");
	    __asm volatile (" nop ");
	    };
	ESXEpiCore::Idle();
	}
      else
	{
        ESXEpiCore::Interrupts::GlobalEnable(false);
	ESXEpiCore::SleepInCore(1,10);
	ESXEpiCore::Timer::Write(E_CTIMER_0,E_CTIMER_MAX);
	uStart = ESXEpiCore::Timer::Start(E_CTIMER_0,E_CTIMER_CLK);
	uStop = ESXEpiCore::Timer::Read(E_CTIMER_0);
	uTare = uStart - uStop;
	//
	//for(i=0;i<ESXMAXSAMPLES;i++)
	for(i=0;i<5000;i++)
	  {
#if	ESXLOOPCOUNT
	  uLoopCount = 0;
#endif
	  uValue = 255 | (ESXEpiCore::Workgroup::CoreId()<<8);
	  ESXEpiCore::mSleepInCore(1,1);
	  *pInt = 0;
	  while(*pInt!=0)
	    ;
	  for(j=0;j<128;j++)
	    {
	    __asm volatile (" nop ");
	    __asm volatile (" nop ");
	    __asm volatile (" nop ");
	    __asm volatile (" nop ");
	    __asm volatile (" nop ");
	    __asm volatile (" nop ");
	    __asm volatile (" nop ");
	    __asm volatile (" nop ");
	    };
	  //ESXEpiCore::mSleepInCore(1,10);
	  ESXEpiCore::Timer::Write(E_CTIMER_1,E_CTIMER_MAX);
	  uStallStart = ESXEpiCore::Timer::Start(E_CTIMER_1,E_CTIMER_EXT_LOAD_STALLS);
          ESXEpiCore::Timer::Write(E_CTIMER_0,E_CTIMER_MAX);
          uStart = ESXEpiCore::Timer::Start(E_CTIMER_0,E_CTIMER_CLK);
	  *pInt = uValue;
	  uTemp0 = *pInt;
#if	ESXTWOREAD
	  if(uTemp0!=uValue)
	    uTemp0 = *pInt;
#endif
	  uStop = ESXEpiCore::Timer::Read(E_CTIMER_0);
	  uStallStop = ESXEpiCore::Timer::Read(E_CTIMER_1);
	  uMeasurement = uStart-uStop-uTare;
	  if(uTemp0!=uValue)
	    uMeasurement = 0xffff;
#if	ESXLOOPCOUNT
	  if(uValue==uTemp0)
	      uLoopCount = 1;
	    else
	      uLoopCount = 0;
#endif
          statsCore.Sample(uMeasurement,uStallStart-uStallStop);
#if	ESXRESULTS
	  uResults[i] = uMeasurement;
#endif
#if	ESXHISTOGRAM & !ESXRESULTS
	  Hist.Insert(uMeasurement);
#endif
#if	ESXSTALLS
	  uStalls[i] = uStallStart-uStallStop;
#endif
#if	ESXLOOPCOUNT
	  uLoopCounts[i] = uLoopCount;
#endif
	  //ESXFATALERROR((uStart-uStop-uTare)-(uStallStart-uStallStop)!=13);
	  //epiPrintF("%u,%u,%u,%u,%u\n",
	  //  uStart,uStop,uTare,uStallStart,uStallStop);
	  ESXFATALERROR(uMeasurement>0xffff);
	  epiWatchDog();
	  };
        ESXEpiCore::Interrupts::GlobalEnable(true);
	WakeUp(uThread);
	};
  BarrierWait();
  //
  for(i=uTestCoreFirst;i<=uTestCoreLast;i++)
    {
    if(i==uThread)
      {
      epiPrintF("CoreId: 0x%08x\n",ESXEpiCore::Workgroup::CoreId());
      epiPrintF("pInt: %p\n",pInt);
      if(bTestOtherCoreMemory)
	{
	epiPrintF("uOtherCoreRow: %d\n",uOtherCoreRow);
	epiPrintF("uOtherCoreCol: %d\n",uOtherCoreCol);
	};
      epiPrintF("    [%2d]%5.1f(%5.1f)",
        uThread,statsCore.Average(),statsCore.StdDev());
      epiPrintF("\n");
      uMin = (uint)-1;
      uMax = 0;
#if	ESXRESULTS
      for(j=0;j<ESXMAXSAMPLES;j++)
	{
	if(bResults)
#if	ESXSTALLS & ESXLOOPCOUNT
	  epiPrintF(TEXT("uResults[%3d]: %5u %5u %5d %5u\n"),
	    j,uResults[j],uStalls[j],uResults[j]-uStalls[j],uLoopCounts[j]);
#elif	ESXSTALLS
	  epiPrintF(TEXT("uResults[%3d]: %5u %5u %5d\n"),
	    j,uResults[j],uStalls[j],uResults[j]-uStalls[j]);
#elif	ESXLOOPCOUNT
	  epiPrintF(TEXT("uResults[%3d]: %5u %5u\n"),
	    j,uResults[j],uLoopCounts[j]);
#else
	  epiPrintF(TEXT("uResults[%3d]: %5u\n"),j,uResults[j]);
#endif
        if(uResults[j]<uMin)
	  uMin = uResults[j];
	if(uMax<uResults[j])
	  uMax = uResults[j];
	};
#endif
#if	ESXHISTOGRAM & ESXRESULTS
      for(j=0;j<ESXMAXSAMPLES;j++)
	Hist.Insert(uResults[j]);
#endif
#if	ESXHISTOGRAM && !ESXRESULTS
      uMin = Hist.Min();
      uMax = Hist.Max();
#endif
      uTotal = 0;
      epiPrintF("uTare: %u\n",uTare);
      epiPrintF("uMin:  %u\n",uMin);
      epiPrintF("uMax:  %u\n",uMax);
#if	ESXHISTOGRAM
      for(j=uMin;j<=uMax;j++)
	if((uHistValue=Hist.ValueCount(j))!=0)
	  {
	  epiPrintF("Hist[%4u]: %4u\n",j,uHistValue);
	  uTotal += uHistValue;
	  };
#endif
      epiPrintF("Total:        %5u\n",uTotal);
      epiPrintF("Avg:           %6.1f\n",statsCore.Average());
      epiPrintF("StdDev:        %6.1f\n",statsCore.StdDev());
      if(bStalls)
	{
      	epiPrintF("Avg-Stalls:    %6.1f\n",statsCore.AverageStalls());
      	epiPrintF("StdDev-Stalls: %6.1f\n",statsCore.StdDevStalls());
	};
      epiPrintF("Mean:          %6.1f\n",Hist.Mean());
      epiPrintF("Median:        %6.1f\n",Hist.Median());
      epiPrintF("Mode:          %4u\n",  Hist.Mode());
      epiPrintF("Usage:         %6.1f%%\n",Hist.Usage());
      };
    BarrierWait();
    };
  //epiPrintF("\n");
  BarrierWait();
  //
  ESXEpiCore::Interrupts::GlobalEnable(false);
  ESXEpiCore::Interrupts::Enable(E_USER_INT,false);
  BarrierWait();
  //
  return(0);
  }

void eSMDTest::WakeUp(uint uThread)
  {
  uint	i;
  uint	j;
  uint	u;
  u = 0;
  for(i=0;i<epiConfigCore.SizeRows();i++)
    for(j=0;j<epiConfigCore.SizeCols();j++)
      {
      if(u!=uThread)
	ESXEpiCore::Interrupts::Generate(E_USER_INT,i,j);
      u++;
      };
  }

void eSMDTest::Help()
  {
  size_t	i;
  if(epiConfigCore.CoreNumber()==0)
    {
    epiPrintF("-debug"   "\tEnable Debug\n");
    epiPrintF("-help"    "\tDisplay Help\n");
    };
  }

void UserInt()
  {
  //epiPrintF("[%2d]UserInt\n",epiConfigCore.CoreNumber());
  e_irq_clear(epiConfigCore.CoreRow(),epiConfigCore.CoreCol(),E_USER_INT);
  //uUserInt += 1;
  }
