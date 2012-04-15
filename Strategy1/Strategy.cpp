/************************************************************************
 * Copyright(c) 2011, One Unified. All rights reserved.                 *
 * email: info@oneunified.net                                           *
 *                                                                      *
 * This file is provided as is WITHOUT ANY WARRANTY                     *
 *  without even the implied warranty of                                *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                *
 *                                                                      *
 * This software may not be used nor distributed without proper license *
 * agreement.                                                           *
 *                                                                      *
 * See the file LICENSE.txt for redistribution information.             *
 ************************************************************************/

#include "StdAfx.h"

//#include <TFHDF5TimeSeries/HDF5DataManager.h>
//#include <TFHDF5TimeSeries/HDF5TimeSeriesContainer.h>

#include <TFTrading/InstrumentManager.h>

#include "Strategy.h"

Strategy::Strategy( pProvider_t pDataProvider, pProvider_t pExecutionProvider ) 
  : 
  m_pDataProvider( pDataProvider ), m_pExecutionProvider( pExecutionProvider ),
  //m_sim( new ou::tf::CSimulationProvider() ),
  m_TradeDirection( ETradeDirUnkn ),
  m_sma1( m_quotes, seconds(   60 ) ), //   1 min
  m_sma2( m_quotes, seconds(  120 ) ), //   2 min
  m_sma3( m_quotes, seconds(  180 ) ), //   3 min
  m_sma4( m_quotes, seconds(  300 ) ), //   5 min
  m_sma5( m_quotes, seconds(  900 ) ), //  15 min
  m_sma6( m_quotes, seconds( 1800 ) ), //  30 min
  m_sma7( m_quotes, seconds( 3600 ) ), //  60 min
  m_sma8( m_quotes, seconds( 7200 ) ), // 120 min
//  m_stateTrade( ETradeOut ), m_dtEnd( date( 2011, 9, 23 ), time_duration( 17, 58, 0 ) ),
  m_stateTrade( ETradeStart ), //m_dtEnd( date( 2011, 11, 7 ), time_duration( 17, 45, 0 ) ),  // put in time start
  m_dtEnd( boost::date_time::not_a_date_time ),
  m_tdTimeBetweenTrades( 0, 0, 0 ), m_dtLastSubmission( boost::date_time::not_a_date_time ),
  m_nUpTransitions( 0 ), m_nDnTransitions( 0 ), 
  m_barFactory( 180 ),
  m_dvChart( "Strategy1", "GC" ),
  m_ceShorts( ou::ChartEntryShape::ESell, ou::Colour::Orange ),
  m_ceLongs( ou::ChartEntryShape::EBuy, ou::Colour::Blue ),
//  m_tsswSlopeOfSlopeOfSMA1( &m_pricesSlopeOfSlopeOfSMA1, 90 ), 
  m_tsswSlopeOfSlopeOfSMA2( m_pricesSlopeOfSlopeOfSMA2, seconds( 180 ) ),
  m_tsswSlopeOfBollinger2Offset( m_pricesBollinger2Offset, seconds( 240 ) ),
  m_tsswSpreads( m_spreads, seconds( 120 ) ),
  m_rtTickDiffs( m_pricesTickDiffs, seconds( 120 ) ),
  m_rocTickDiffs( m_pricesTickDiffsROC, seconds( 30 ) ),
  m_bFirstTrade( true ),
  m_dblUpTicks( 0.0 ), m_dblMdTicks( 0.0 ), m_dblDnTicks( 0.0 ),
  m_dblUpVolume( 0.0 ), m_dblMdVolume( 0.0 ), m_dblDnVolume( 0.0 )
{

  assert( m_pDataProvider->Connected() && m_pExecutionProvider->Connected() );

  m_tdTimeBetweenTrades += boost::posix_time::millisec( 30 );

//  ou::tf::CProviderManager::Instance().Register( "data01", m_pDataProvider );
//  ou::tf::CProviderManager::Instance().Register( "exec01", m_pExecutionProvider );

  m_dvChart.Add( 0, m_ceZigZag );
  m_dvChart.Add( 0, m_ceShorts );
  m_dvChart.Add( 0, m_ceLongs );
  m_dvChart.Add( 0, m_ceBars );
//  m_dvChart.Add( 0, m_ceSMA1 );
  m_dvChart.Add( 0, m_ceSMA2 );
  m_dvChart.Add( 0, m_ceSMA3 );
//  m_dvChart.Add( 0, m_ceUpperBollinger1 );
//  m_dvChart.Add( 0, m_ceLowerBollinger1 );
  m_dvChart.Add( 0, m_ceUpperBollinger2 );
  m_dvChart.Add( 0, m_ceLowerBollinger2 );
  m_dvChart.Add( 0, m_ceUpperBollinger3 );
  m_dvChart.Add( 0, m_ceLowerBollinger3 );
  m_dvChart.Add( 1, m_ceVolume );
//  m_dvChart.Add( 2, m_ceSlopeOfSMA1 );
//  m_dvChart.Add( 2, m_ceSlopeOfSlopeOfSMA1 );
//  m_dvChart.Add( 2, m_ceSMA2RR );
//  m_dvChart.Add( 2, m_ceTickDiffsRoc );
//  m_dvChart.Add( 2, m_ceSMA1RR );
/*  m_dvChart.Add( 2, m_ceSMA2RR );
  m_dvChart.Add( 2, m_ceSMA3RR );
  m_dvChart.Add( 4, m_ceSlopeOfSMA2 );
  m_dvChart.Add( 4, m_ceSlopeOfSMA3 );
  m_dvChart.Add( 4, m_ceSlopeOfSlopeOfSMA2 );
//  m_dvChart.Add( 4, m_ceSlopeOfBollinger2Offset );
//  m_dvChart.Add( 4, m_ceBollinger1Offset );
  m_dvChart.Add( 5, m_ceBollinger2Offset );
  m_dvChart.Add( 5, m_ceBollinger3Offset );
  m_dvChart.Add( 6, m_ceOutstandingEntriesLong );
  m_dvChart.Add( 6, m_ceOutstandingEntriesShort );
  m_dvChart.Add( 6, m_ceOutstandingExitsLong );
  m_dvChart.Add( 6, m_ceOutstandingExitsShort );
//  m_dvChart.Add( 3, m_ceRR );
  m_dvChart.Add( 7, m_cePLLong );
  m_dvChart.Add( 7, m_cePLShort );
  m_dvChart.Add( 7, m_cePLNet ); */
//  m_dvChart.Add( 5, m_ceSpread );

//  m_dvChart.Add( 8, m_ceBollinger3Ratio );
//  m_dvChart.Add( 8, m_ceBollinger2Ratio );
//  m_dvChart.Add( 8, m_ceBollinger1Ratio );

//  m_ceSlopeOfSMA1.SetName( "SlopeOfSMA1" );
//  m_ceSlopeOfSlopeOfSMA1.SetName( "SlopeOfSlopeOfSMA1" );

//  m_ceSlopeOfSMA1.SetName( "SlopeOfSMA1" );
  m_ceSlopeOfSlopeOfSMA2.SetName( "SlopeOfSlopeOfSMA2" );
  m_ceSlopeOfBollinger2Offset.SetName( "SlopeOfBollinger2Offset" );

  m_cePLNet.SetName( "PL Net" );

//  m_ceBollinger1Offset.SetName( "Bollinger1Offset" );
  m_ceBollinger2Offset.SetName( "Bollinger2Offset" );
  m_ceBollinger3Offset.SetName( "Bollinger3Offset" );

//  m_ceSMA1.SetColour( ou::Colour::DarkOliveGreen );
  m_ceSMA2.SetColour( ou::Colour::Turquoise );
  m_ceSMA3.SetColour( ou::Colour::GreenYellow );

//  m_ceSMA1RR.SetColour( ou::Colour::Yellow );
  m_ceSMA2RR.SetColour( ou::Colour::Turquoise );
  m_ceSMA3RR.SetColour( ou::Colour::GreenYellow );

  m_cePLLong.SetColour( ou::Colour::Blue );
  m_cePLShort.SetColour( ou::Colour::Orange );
  m_cePLNet.SetColour( ou::Colour::Green );

//  m_ceSlopeOfSMA1.SetColour( ou::Colour::DarkOliveGreen );
  m_ceSlopeOfSMA2.SetColour( ou::Colour::Turquoise );
  m_ceSlopeOfSMA3.SetColour( ou::Colour::GreenYellow );

//  m_ceSlopeOfSlopeOfSMA1.SetColour( ou::Colour::MediumAquamarine );
  m_ceSlopeOfSlopeOfSMA2.SetColour( ou::Colour::Green );

  m_ceOutstandingEntriesLong.SetColour( ou::Colour::Blue );
  m_ceOutstandingEntriesShort.SetColour( ou::Colour::Red );
  m_ceOutstandingExitsLong.SetColour( ou::Colour::DarkCyan );
  m_ceOutstandingExitsShort.SetColour( ou::Colour::OrangeRed );

//  m_ceBollinger1Ratio.SetColour( ou::Colour::DarkOliveGreen );
//  m_ceBollinger2Ratio.SetColour( ou::Colour::Turquoise );
//  m_ceBollinger3Ratio.SetColour( ou::Colour::GreenYellow );

  m_ceUpperBollinger1.SetColour( ou::Colour::DarkOliveGreen );
  m_ceLowerBollinger1.SetColour( ou::Colour::DarkOliveGreen );
  m_ceUpperBollinger2.SetColour( ou::Colour::Turquoise );
  m_ceLowerBollinger2.SetColour( ou::Colour::Turquoise );
  m_ceUpperBollinger3.SetColour( ou::Colour::GreenYellow );
  m_ceLowerBollinger3.SetColour( ou::Colour::GreenYellow );

  m_ceBollinger1Offset.SetColour( ou::Colour::DarkOliveGreen );
  m_ceBollinger2Offset.SetColour( ou::Colour::Turquoise );
  m_ceBollinger3Offset.SetColour( ou::Colour::GreenYellow );

  m_ceSlopeOfBollinger2Offset.SetColour( ou::Colour::DarkMagenta );

  m_ceTickDiffs.SetColour( ou::Colour::Turquoise );
  m_ceTickDiffsRoc.SetColour( ou::Colour::Purple );

  m_ceZigZag.SetColour( ou::Colour::DarkBlue );

  m_zigzagPrice.SetOnPeakFound( MakeDelegate( this, &Strategy::HandleZigZagPeak ) );
  m_zigzagPrice.SetUpDecisionPointFound( MakeDelegate( this, &Strategy::HandleZigZagUpDp ) );
  m_zigzagPrice.SetDnDecisionPointFound( MakeDelegate( this, &Strategy::HandleZigZagDnDp ) );

  m_barFactory.SetOnBarComplete( MakeDelegate( this, &Strategy::HandleBarCompletion ) );

  ou::tf::CInstrumentManager& mgr( ou::tf::CInstrumentManager::Instance() );
  m_pTestInstrument = mgr.Exists( "+GCZ11" ) ? mgr.Get( "+GCZ11" ) : mgr.ConstructFuture( "+GCZ11", "SMART", 2011, 12 );
  m_pTestInstrument->SetMultiplier( 100 );
  m_pTestInstrument->SetMinTick( 0.1 );

}

Strategy::~Strategy(void) {

  m_barFactory.SetOnBarComplete( 0 );

}

void Strategy::Start( void ) {  // live trading
}

void Strategy::Start( const std::string& sSymbolPath ) {  // simulated trading

  m_sim = boost::dynamic_pointer_cast<ou::tf::CSimulationProvider>( m_pExecutionProvider );

  //m_sim->SetGroupDirectory( "/semiauto/2011-Sep-23 19:17:48.252497" );
  m_sim->SetGroupDirectory( "/app/semiauto/2011-Nov-06 18:54:22.184889" );
  //m_sim->SetGroupDirectory( "/app/semiauto/2011-Nov-07 18:53:31.016760" );
  //m_sim->SetGroupDirectory( "/app/semiauto/2011-Nov-08 18:58:29.396624" );
//  m_sim->SetExecuteAgainst( ou::tf::CSimulateOrderExecution::EAQuotes );

  m_dtEnd = boost::posix_time::ptime( date( 2011, 11, 7 ), time_duration( 17, 45, 0 ) );  // put in time start
  
  m_sim->SetOnSimulationComplete( MakeDelegate( this, &Strategy::HandleSimulationComplete ) );

  Activate();

  m_sim->Run();
}

void Strategy::Activate( void ) {

  // all this needs to run together, as the group directory from above required for symbol access
  m_pPositionLong.reset( new ou::tf::CPosition( m_pTestInstrument, m_pExecutionProvider, m_pDataProvider ) );
  m_pPositionLong->OnExecution.Add( MakeDelegate( this, &Strategy::HandleExecution ) );
  m_pPositionLong->OnCommission.Add( MakeDelegate( this, &Strategy::HandleCommission ) );

  m_pOrdersOutstandingLongs = new OrdersOutstandingLongs( m_pPositionLong );

  m_pPositionShort.reset( new ou::tf::CPosition( m_pTestInstrument, m_pExecutionProvider, m_pDataProvider ) );
  m_pPositionShort->OnExecution.Add( MakeDelegate( this, &Strategy::HandleExecution ) );
  m_pPositionShort->OnCommission.Add( MakeDelegate( this, &Strategy::HandleCommission ) );

  m_pOrdersOutstandingShorts = new OrdersOutstandingShorts( m_pPositionShort );

  // things screw up if these two lines come last
  m_pDataProvider->AddQuoteHandler( m_pTestInstrument, MakeDelegate( this, &Strategy::HandleQuote ) );
  m_pDataProvider->AddTradeHandler( m_pTestInstrument, MakeDelegate( this, &Strategy::HandleTrade ) );
  //m_pDataProvider->AddQuoteHandler( m_pTestInstrument, MakeDelegate( this, &Strategy::HandleFirstQuote ) );
  //m_pDataProvider->AddTradeHandler( m_pTestInstrument, MakeDelegate( this, &Strategy::HandleFirstTrade ) );

}

void Strategy::HandleQuote( const ou::tf::CQuote& quote ) {

  if ( !quote.IsValid() ) {
    return;
  }
  // should also check that a price within 2 - 3 sigma of last

  // problems occur when long trend happens and can't get out of opposing position.

  ptime dt( quote.DateTime() );

  m_quoteLast = quote;
  m_quotes.Append( quote );

  // high speed simple moving average
  ou::tf::TSSWStatsMidQuote& sma1( m_sma4 );
//  sma1.Update();

//  m_ceSMA1RR.Add( dt, sma1.RR() );

//  m_pricesSlopeOfSlopeOfSMA1.Append( ou::tf::CPrice( dt, sma1.Slope() ) );
//  m_tsswSlopeOfSlopeOfSMA1.Update();

  // medium speed moving average
  ou::tf::TSSWStatsMidQuote& sma2( m_sma6 );
//  sma2.Update();

  double dblSMA2RR = sma2.RR();
  m_ceSMA2RR.Add( dt, dblSMA2RR );

  m_pricesBollinger2Offset.Append( ou::tf::CPrice( dt, sma2.BBOffset() ) );
//  m_tsswSlopeOfBollinger2Offset.Update();

  // slow speed moving average
  ou::tf::TSSWStatsMidQuote& sma3( m_sma7 );
//  sma3.Update();

  m_ceSMA3RR.Add( dt, sma3.RR() );

//  double spread = quote.Ask() - quote.Bid();
//  m_spreads.Append( ou::tf::CPrice( dt, spread ) );
//  m_tsswSpreads.Update();

  if ( 500 < m_quotes.Size() ) {

    m_pricesSlopeOfSlopeOfSMA2.Append( ou::tf::CPrice( dt, sma2.Slope() ) );
//    m_tsswSlopeOfSlopeOfSMA2.Update();

    m_pOrdersOutstandingLongs->HandleQuote( quote );
    m_pOrdersOutstandingShorts->HandleQuote( quote );

//    m_ceSpread.Add( dt, m_tsswSpreads.MeanY() );

    unsigned int cntLongs = m_pOrdersOutstandingLongs->GetCountOfOutstandingMatches();
    m_ceOutstandingEntriesLong.Add( dt, cntLongs );
    unsigned int cntShorts = m_pOrdersOutstandingShorts->GetCountOfOutstandingMatches();
    m_ceOutstandingEntriesShort.Add( dt, cntShorts );
    unsigned int dif = ( cntLongs > cntShorts ) ? cntLongs - cntShorts : cntShorts - cntLongs;

    m_ceOutstandingExitsLong.Add( dt, cntLongs + m_pOrdersOutstandingLongs->GetCountOfOutstandingEntries() );
    m_ceOutstandingExitsShort.Add( dt, cntShorts + m_pOrdersOutstandingShorts->GetCountOfOutstandingEntries() );

//    m_ceSMA1.Add( dt, sma1.MeanY() );

//    double slope1 = sma1.Slope();
//    if ( 0.04 < slope1 ) { slope1 = 0.04; } else { if ( -0.04 > slope1 ) slope1 = -0.04; };

//    m_ceSlopeOfSMA1.Add( dt, slope1 );
//    double direction1 = m_tsswSlopeOfSlopeOfSMA1.Slope();
//    if ( ( 0.00008 < direction1 ) || ( -0.00008 > direction1 ) ) {
//      direction1 = 0.00008;
//    }
//    else {
//      m_ceSlopeOfSlopeOfSMA1.Add( dt, direction1 * 400.0 );
 //   }

//    m_ceUpperBollinger1.Add( dt, sma1.BBUpper() );
//    m_ceLowerBollinger1.Add( dt, sma1.BBLower() );
//    m_ceBollinger1Offset.Add( dt, sma1.BBOffset() );

    double slope2 = sma2.Slope();

    m_ceSMA2.Add( dt, sma2.MeanY() );
    m_ceSlopeOfSMA2.Add( dt, slope2 * 10.0 );
    m_ceUpperBollinger2.Add( dt, sma2.BBUpper() );
    m_ceLowerBollinger2.Add( dt, sma2.BBLower() );
    m_ceBollinger2Offset.Add( dt, sma2.BBOffset() );

    double direction2 = m_tsswSlopeOfSlopeOfSMA2.Slope();
    if ( ( 0.00001 > direction2 ) || ( -0.00001 < direction2 ) ) {
//      direction2 = 0.0;
    }
    else {
      
    }
    m_ceSlopeOfSlopeOfSMA2.Add( dt, direction2 * 2000.0 );

    double dblBollingerSlope = m_tsswSlopeOfBollinger2Offset.Slope();
    m_ceSlopeOfBollinger2Offset.Add( dt, dblBollingerSlope * 2.0 );

    m_ceSMA3.Add( dt, sma3.MeanY() );
    m_ceSlopeOfSMA3.Add( dt, sma3.Slope() * 10.0 );
    m_ceUpperBollinger3.Add( dt, sma3.BBUpper() );
    m_ceLowerBollinger3.Add( dt, sma3.BBLower() );
    m_ceBollinger3Offset.Add( dt, sma3.BBOffset() );

    //m_ceRR.Add( quote.DateTime(), m_sma5min.RR() );
    double dblPLLong = m_pPositionLong->GetRealizedPL() + m_pPositionLong->GetUnRealizedPL() - m_pPositionLong->GetCommissionPaid();
    double dblPLShort = m_pPositionShort->GetRealizedPL() + m_pPositionShort->GetUnRealizedPL() - m_pPositionShort->GetCommissionPaid();
    m_cePLLong.Add( dt, dblPLLong );
    m_cePLShort.Add( dt, dblPLShort );
    m_cePLNet.Add( dt, dblPLLong + dblPLShort );

    double midpoint = quote.Midpoint();
//    double /* val1,*/ val2, val3;
//    val1 = ( midpoint - sma1.MeanY() ) / ( sma1.BBOffset() );
//    if ( 1.1 < val1 ) val1 = 1.1;
//    else 
//      if ( -1.1 > val1 ) val1 = -1.1;
//    m_ceBollinger1Ratio.Add( dt, val1 );
/*
    val2 = ( midpoint - sma2.MeanY() ) / ( sma2.BBOffset() );
    if ( 1.1 < val2 ) val2 = 1.1;
    else 
      if ( -1.1 > val2 ) val2 = -1.1;
    m_ceBollinger2Ratio.Add( dt, val2 );

    val3 = ( midpoint - sma3.MeanY() ) / ( sma3.BBOffset() );
    if ( 1.1 < val3 ) val3 = 1.1;
    else 
      if ( -1.1 > val3 ) val3 = -1.1;
    m_ceBollinger3Ratio.Add( dt, val3 );
*/
    typedef OrdersOutstanding::structRoundTrip structRoundTrip;

    if ( dt > m_dtEnd ) {
      m_stateTrade = ETradeCancel;
    }

    switch ( m_stateTrade ) {
    case ETradeStart:
      m_stateTrade = ETradeAbove21Starting;
      break;
    case ETradeAbove21Starting:
      if ( 0.21 < dblSMA2RR ) {
        m_stateTrade = ETradeAbove21Ending;
      }
      break;
    case ETradeAbove21Ending:
      if ( 0.19 > dblSMA2RR ) {
        m_stateTrade = ETradeBelow19;
      }
      break;
    case ETradeAbove21:
      break;
    case ETradeBelow19:
      m_dblLastMidpoint = midpoint;
      m_dtLastSubmission = quote.DateTime();
      if ( 0.0 < direction2 ) {
        m_stateTrade = ETradeLong;
      }
      else {
        if ( 0.0 > direction2 ) {
          m_stateTrade = ETradeShort;
        }
      }
      break;
    case ETradeLong:
      {
        bool bTimeToExit( false );
//        if ( 5 > dblSMA2RR ) {
//          m_stateTrade = ETradeBelow05;
//          bTimeToExit = true;
//        }
        if ( 0.60 < dblSMA2RR ) {
          m_stateTrade = ETradeAbove60;
          bTimeToExit = true;
        }
        if ( 0.0 > direction2 ) {
          m_stateTrade = ETradeAbove60;
          bTimeToExit = true;
        }
        if ( !bTimeToExit ) {
          if ( ( ( m_dblLastMidpoint + 0.010 ) < midpoint ) && ( 0 == cntLongs ) ) {
            bool bAllowTrade( true );
            if ( !m_dtLastSubmission.is_not_a_date_time() ) 
              bAllowTrade = (m_dtLastSubmission + m_tdTimeBetweenTrades ) <= quote.DateTime();
            if ( bAllowTrade ) {
              m_pOrdersOutstandingLongs->CancelAllButNEntryOrders( 2 );
              double dblNormalized = m_pTestInstrument->NormalizeOrderPrice( quote.Midpoint() - 0.10 );
              m_pOrder = m_pPositionLong->
                PlaceOrder( ou::tf::OrderType::Limit, ou::tf::OrderSide::Buy, 1, dblNormalized );
              m_pOrder->OnOrderFilled.Add( MakeDelegate( this, &Strategy::HandleOrderFilled ) );
              m_pOrdersOutstandingLongs->AddOrderFilling( 
                new structRoundTrip( m_pOrder, dblNormalized + 0.2, m_pTestInstrument->NormalizeOrderPrice(sma2.MeanY() ) ) );
              m_pOrder.reset();
              ++m_nUpTransitions;
              m_dblLastMidpoint = midpoint;
              m_dtLastSubmission = quote.DateTime();
            }
          }
        }
      }
      break;
    case ETradeShort:
      {
        bool bTimeToExit( false );
//        if ( 5 > dblSMA2RR ) {
//          m_stateTrade = ETradeBelow05;
//          bTimeToExit = true;
//        }
        if ( 0.60 < dblSMA2RR ) {
          m_stateTrade = ETradeAbove60;
          bTimeToExit = true;
        }
        if ( 0.0 < direction2 ) {
          m_stateTrade = ETradeAbove60;
          bTimeToExit = true;
        }
        if ( !bTimeToExit ) {
          if ( ( ( m_dblLastMidpoint + 0.010 ) > midpoint ) && ( 0 == cntShorts ) ) {
            bool bAllowTrade( true );
            if ( !m_dtLastSubmission.is_not_a_date_time() ) 
              bAllowTrade = (m_dtLastSubmission + m_tdTimeBetweenTrades ) <= quote.DateTime();
            if ( bAllowTrade ) {
              m_pOrdersOutstandingShorts->CancelAllButNEntryOrders( 2 );
              double dblNormalized = m_pTestInstrument->NormalizeOrderPrice( quote.Midpoint() + 0.10 );
              m_pOrder = m_pPositionShort->
                PlaceOrder( ou::tf::OrderType::Limit, ou::tf::OrderSide::Sell, 1, dblNormalized );
              m_pOrder->OnOrderFilled.Add( MakeDelegate( this, &Strategy::HandleOrderFilled ) );
              m_pOrdersOutstandingShorts->AddOrderFilling( 
                new structRoundTrip( m_pOrder, dblNormalized - 0.2, m_pTestInstrument->NormalizeOrderPrice( sma2.MeanY() ) ) );
              m_pOrder.reset();
              ++m_nDnTransitions;
              m_dblLastMidpoint = midpoint;
              m_dtLastSubmission = quote.DateTime();
            }
          }
        }
      }
      break;
    case ETradeBelow05:
      break;
    case ETradeAbove60:
      m_stateTrade = ETradeAbove21Starting;
      break;
    case ETradeWaitForTrendConfirmation:
      if ( 0.0 > dblBollingerSlope ) { // reset for another attempt if falling
        m_stateTrade = ETradeWaitForBollingerToRise;
      }
      else {  // ensure enough indicators are pointing in the correct direction
        if ( 0.0 < slope2 ) {
          m_stateTrade = ETradeLongAndWaitForCrossingSMADownwards;
        }
        if ( 0.0 > slope2 ) {
          m_stateTrade = ETradeShortAndWaitForCrossingSMAUpwards;
        }
      }
      break;
    case ETradeLongAndWaitForCrossingSMADownwards:  // long oriented
      if ( ( 0.0 > dblBollingerSlope) /*|| ( 0.0 > m_rtTickDiffs.Net() )*/ ) { // reset for another attempt
        // stop out stuff here 
        m_pOrdersOutstandingLongs->CancelAndCloseAllOrders();
        m_stateTrade = ETradeWaitForBollingerToRise;
      }
      else {
        if ( ( 0.0 < slope2 ) && ( sma1.MeanY() > sma2.MeanY() ) && ( ( m_dblLastMidpoint + 0.010 ) < midpoint ) 
            //&& ( 0.0 < m_rocTickDiffs.RateOfChange() )
            && ( 0 == cntLongs ) /*&& ( 2.0 < sma2.BBOffset() )*/ ) {
          bool bAllowTrade( true );
          if ( !m_dtLastSubmission.is_not_a_date_time() ) 
            bAllowTrade = (m_dtLastSubmission + m_tdTimeBetweenTrades ) <= quote.DateTime();
          if ( bAllowTrade ) {
            m_pOrdersOutstandingLongs->CancelAllButNEntryOrders( 2 );
            double dblNormalized = m_pTestInstrument->NormalizeOrderPrice( quote.Midpoint() - 0.10 );
            m_pOrder = m_pPositionLong->
              PlaceOrder( ou::tf::OrderType::Limit, ou::tf::OrderSide::Buy, 1, dblNormalized );
            m_pOrder->OnOrderFilled.Add( MakeDelegate( this, &Strategy::HandleOrderFilled ) );
            m_pOrdersOutstandingLongs->AddOrderFilling( 
              new structRoundTrip( m_pOrder, dblNormalized + 0.2, m_pTestInstrument->NormalizeOrderPrice(sma2.MeanY() ) ) );
            m_pOrder.reset();
            ++m_nUpTransitions;
            m_dblLastMidpoint = midpoint;
            m_dtLastSubmission = quote.DateTime();
          }
        }
        else {
          if ( sma1.MeanY() < sma2.MeanY() ) {
            m_stateTrade = ETradeWaitForBollingerToRise;
          }
        }
      }
      break;
    case ETradeShortAndWaitForCrossingSMAUpwards:  // short oriented
      if ( ( 0.0 > dblBollingerSlope) /*|| ( 0.0 < m_rtTickDiffs.Net() )*/ ) { // reset for another attempt
        // stop out stuff here 
        m_pOrdersOutstandingShorts->CancelAndCloseAllOrders();
        m_stateTrade = ETradeWaitForBollingerToRise;
      }
      else {
        if ( ( 0.0 > slope2 ) && ( sma1.MeanY() < sma2.MeanY() ) && ( ( m_dblLastMidpoint - 0.010 ) > midpoint ) 
            //&& ( 0.0 > m_rocTickDiffs.RateOfChange() )
            && ( 0 == cntShorts ) /*&& ( 2.0 < sma2.BBOffset() )*/ ) {
          bool bAllowTrade( true );
          if ( !m_dtLastSubmission.is_not_a_date_time() ) 
            bAllowTrade = (m_dtLastSubmission + m_tdTimeBetweenTrades ) <= quote.DateTime();
          if ( bAllowTrade ) {
            m_pOrdersOutstandingShorts->CancelAllButNEntryOrders( 2 );
            double dblNormalized = m_pTestInstrument->NormalizeOrderPrice( quote.Midpoint() + 0.10 );
            m_pOrder = m_pPositionShort->
              PlaceOrder( ou::tf::OrderType::Limit, ou::tf::OrderSide::Sell, 1, dblNormalized );
            m_pOrder->OnOrderFilled.Add( MakeDelegate( this, &Strategy::HandleOrderFilled ) );
            m_pOrdersOutstandingShorts->AddOrderFilling( 
              new structRoundTrip( m_pOrder, dblNormalized - 0.2, m_pTestInstrument->NormalizeOrderPrice( sma2.MeanY() ) ) );
            m_pOrder.reset();
            ++m_nDnTransitions;
            m_dblLastMidpoint = midpoint;
            m_dtLastSubmission = quote.DateTime();
          }
        }
        else {
          if ( sma1.MeanY() > sma2.MeanY() ) {
            m_stateTrade = ETradeWaitForBollingerToRise;
          }
        }
      }
      break;
    case ETradeWaitForBollingerToRise:
      if ( 0.0 < dblBollingerSlope ) {
        m_stateTrade = ETradeWaitForTrendConfirmation;
      }
      break;
    case ETradeWaitForBollingerToFall:
      if ( 0.0 > dblBollingerSlope ) {
        m_dblLastMidpoint = midpoint;
        m_stateTrade = ETradeWaitForBollingerToRise;
        //m_stateTrade = ETradeWaitForUp;
      }
      break;
    case ETradeCancel:
      m_pOrdersOutstandingLongs->CancelAndCloseAllOrders();
      m_pOrdersOutstandingShorts->CancelAndCloseAllOrders();
      m_stateTrade = ETradeClose;
      break;
    case ETradeClose:
      m_stateTrade = ETradeDone;
      break;
    case ETradeDone:
      break;
    }
  }
}

void Strategy::HandleOrderFilled( const ou::tf::COrder& order ) {
  switch ( order.GetOrderSide() ) {
  case ou::tf::OrderSide::Sell:
    m_ceShorts.AddLabel( order.GetDateTimeOrderFilled(), order.GetAverageFillPrice(), "" );
    break;
  case ou::tf::OrderSide::Buy:
    m_ceLongs.AddLabel( order.GetDateTimeOrderFilled(), order.GetAverageFillPrice(), "" );
    break;
  }
  const_cast<ou::tf::COrder&>( order ).OnOrderFilled.Remove( MakeDelegate( this, &Strategy::HandleOrderFilled ) );
}

void Strategy::HandleFirstQuote( const ou::tf::CQuote& quote ) {

  if ( !quote.IsValid() ) {
    return;
  }

  m_quoteLast = quote;  // ensure we have a quote before we start to evaluate trades

  m_sim->RemoveQuoteHandler( m_pTestInstrument, MakeDelegate( this, &Strategy::HandleFirstQuote ) );
  m_sim->AddQuoteHandler( m_pTestInstrument, MakeDelegate( this, &Strategy::HandleQuote ) );
  m_sim->RemoveTradeHandler( m_pTestInstrument, MakeDelegate( this, &Strategy::HandleFirstTrade ) );
  m_sim->AddTradeHandler( m_pTestInstrument, MakeDelegate( this, &Strategy::HandleTrade ) );
}

void Strategy::HandleFirstTrade( const ou::tf::CTrade& trade ) {
}

void Strategy::HandleTrade( const ou::tf::CTrade& trade ) {

  ptime dt( trade.DateTime() );
  ou::tf::CTrade::price_t price = trade.Trade();

  if ( m_bFirstTrade ) {
    m_bFirstTrade = false;
    //m_zigzagPrice.SetFilterWidth( 0.17 * sqrt( trade.Trade() ) );
    m_zigzagPrice.SetFilterWidth( 4.0 );
  }

  m_trades.Append( trade );
  m_barFactory.Add( trade );

  m_zigzagPrice.Check( dt, price );

  double mid = m_quoteLast.Midpoint();
  if ( price == mid ) {
    switch ( m_TradeDirection ) {
    case ETradeDirUnkn:
      m_TradeDirection = ETradeDirUp;  // default to up
      //break; fall through instead
    case ETradeDirUp:  // leave as is
      break;
    case ETradeDirDn: // leave as is
      break;
    }
    ++m_dblMdTicks;
    //m_dblMdVolume += trade.Volume();
  }
  else {
    if ( price > mid ) {
      m_TradeDirection = ETradeDirUp;   // definitively up
      //++m_dblMdTicks;
      m_pricesTickDiffs.Append( ou::tf::CPrice( dt, +1.0 ) );
      ++m_dblUpTicks;
      m_dblUpVolume += trade.Volume();
    }
    else {
      m_TradeDirection = ETradeDirDn;  // definitively dow
      //--m_dblMdTicks;
      m_pricesTickDiffs.Append( ou::tf::CPrice( dt, -1.0 ) );
      ++m_dblDnTicks;
      m_dblDnVolume += trade.Volume();
    }
  }

//  m_rtTickDiffs.Update();
  double dif = m_rtTickDiffs.Net();
  m_pricesTickDiffsROC.Append( ou::tf::CPrice( dt, dif ) );

  if (  45 < dif ) dif = 45;
  if ( -45 > dif ) dif = -45;
  m_ceTickDiffs.Add( dt, dif );

//  m_rocTickDiffs.Update();
  dif = m_rocTickDiffs.RateOfChange();
  if (  45 < dif ) dif = 45;
  if ( -45 > dif ) dif = -45;
  m_ceTickDiffsRoc.Add( dt, dif );
}

void Strategy::HandleSimulationComplete( void ) {

  m_pOrdersOutstandingLongs->PostMortemReport();
  m_pOrdersOutstandingShorts->PostMortemReport();
  m_ss.str( "" );
  m_ss << m_nUpTransitions << " up changes, ";
  m_ss << m_nDnTransitions << " dn changes, ";
  m_pPositionLong->EmitStatus( m_ss );
  m_ss << ", ";
  m_pPositionShort->EmitStatus( m_ss );
  m_ss << ". ";
  m_sim->EmitStats( m_ss );
  std::cout << m_ss << std::endl;

  m_sim->SetOnSimulationComplete( 0 );
  m_sim->RemoveQuoteHandler( m_pTestInstrument, MakeDelegate( this, &Strategy::HandleQuote ) );
  m_sim->RemoveTradeHandler( m_pTestInstrument, MakeDelegate( this, &Strategy::HandleTrade ) );

}

void Strategy::HandleExecution( ou::tf::CPosition::execution_delegate_t del ) {
  m_ss << "Exec: " << del.second.GetTimeStamp() << ": ";
  m_pPositionLong->EmitStatus( m_ss );
  m_ss << ", ";
  m_pPositionShort->EmitStatus( m_ss );
  std::cout << m_ss << std::endl;
}

void Strategy::HandleCommission( const ou::tf::CPosition* pPosition ) {
  m_ss.str( "" );
  m_pPositionLong->EmitStatus( m_ss );
  m_ss << ", ";
  m_pPositionShort->EmitStatus( m_ss );
  std::cout << m_ss << std::endl;
}

void Strategy::HandleBarCompletion( const ou::tf::CBar& bar ) {

  ptime dt( bar.DateTime() );
  m_ceBars.AddBar( bar );

  m_ceVolume.Add( dt, bar.Volume() );

}

void Strategy::HandleZigZagPeak( ou::tf::ZigZag*, ptime dt, double price, ou::tf::ZigZag::EDirection ) {
  m_ceZigZag.Add( dt, price );
  //m_zigzagPrice.SetFilterWidth( 0.17 * sqrt( price ) );
}

void Strategy::HandleZigZagUpDp( ou::tf::ZigZag* ) {
}

void Strategy::HandleZigZagDnDp( ou::tf::ZigZag* ) {
}

