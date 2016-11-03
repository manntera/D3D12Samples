﻿//-------------------------------------------------------------------------------------------------
// File : asdxTimer.h
// Desc : Timer Module.
// Copyright(c) Project Asura. All right reserved.
//-------------------------------------------------------------------------------------------------

#ifndef _ASDX_TIMER_H_
#define _ASDX_TIMER_H_

//-------------------------------------------------------------------------------------------------
// Includes
//-------------------------------------------------------------------------------------------------
#include <Windows.h>
#include <asdxTypedef.h>


namespace asdx {

///////////////////////////////////////////////////////////////////////////////////////////////////
// Timer class
///////////////////////////////////////////////////////////////////////////////////////////////////
class Timer
{
    //=============================================================================================
    // list of friend classes and methods.
    //=============================================================================================
    /* NOTHING */

private:
    //=============================================================================================
    // private variables
    //=============================================================================================
    bool    m_IsStop;               //!< 停止状態かどうか.
    s64     m_TicksPerSec;          //!< 1秒あたりのタイマー刻み数.
    s64     m_StopTime;             //!< 停止時間.
    s64     m_ElapsedTime;          //!< 最後の処理から経過時間です.
    s64     m_BaseTime;             //!< タイマーの開始時間です.
    f64     m_InvTicksPerSec;       //!< 1タイマー刻み数当たりの秒数.

    //=============================================================================================
    // private methods
    //=============================================================================================

    //---------------------------------------------------------------------------------------------
    //! @brief      調整された現在時間を取得します.
    //---------------------------------------------------------------------------------------------
    s64 GetAdjustedCurrentTime( void )
    {
        LARGE_INTEGER qwTime;

        // 停止状態であれば，停止時間を返却.
        if ( m_StopTime != 0 )
        { qwTime.QuadPart = m_StopTime; }
        // 非停止状態ならば，現在のカウンタを取得.
        else
        { QueryPerformanceCounter( &qwTime ); }

        return qwTime.QuadPart;
    }

protected:
    //=============================================================================================
    // protected variables
    //=============================================================================================
    /* NOTHING */

    //=============================================================================================
    // protected methods
    //=============================================================================================
    /* NOTHING */

public:
    //=============================================================================================
    // private variables
    //=============================================================================================
    /* NOTHING */

    //=============================================================================================
    // private methods
    //=============================================================================================

    //---------------------------------------------------------------------------------------------
    //! @brief      コンストラクタです.
    //---------------------------------------------------------------------------------------------
    Timer::Timer()
    : m_IsStop     ( true )
    , m_StopTime   ( 0 )
    , m_ElapsedTime( 0 )
    , m_BaseTime   ( 0 )
    {
        LARGE_INTEGER qwTicksPerSec = { 0 };

        // 周波数を取得します.
        QueryPerformanceFrequency( &qwTicksPerSec );

        m_TicksPerSec = qwTicksPerSec.QuadPart;
        m_InvTicksPerSec = 1.0 / static_cast<double>( m_TicksPerSec );
    }

    //---------------------------------------------------------------------------------------------
    //! @brief      デストラクタです.
    //---------------------------------------------------------------------------------------------
    ~Timer()
    { /* DO_NOTHING */ }

    //---------------------------------------------------------------------------------------------
    //! @brief      タイマーをリセットします.
    //---------------------------------------------------------------------------------------------
    void Reset()
    {
        // 調整された現在時間を取得.
        s64 qwTime = GetAdjustedCurrentTime();

        m_BaseTime    = qwTime;
        m_ElapsedTime = qwTime;
        m_StopTime    = 0;
        m_IsStop      = false;
    }

    //---------------------------------------------------------------------------------------------
    //! @brief      タイマーを開始します.
    //---------------------------------------------------------------------------------------------
    void Start()
    {
        LARGE_INTEGER qwTime = { 0 };

        // 現在のカウンタを取得.
        QueryPerformanceCounter( &qwTime );

        // 停止中ならベース時間を加算.
        if ( m_IsStop )
        { m_BaseTime += qwTime.QuadPart - m_StopTime; }

        m_StopTime    = 0;
        m_ElapsedTime = qwTime.QuadPart;
        m_IsStop      = false;
    }

    //---------------------------------------------------------------------------------------------
    //! @brief      タイマーを停止します.
    //---------------------------------------------------------------------------------------------
    void Stop()
    {
        if ( !m_IsStop )
        {
            LARGE_INTEGER qwTime = { 0 };

            // 現在のカウンタを取得.
            QueryPerformanceCounter( &qwTime );

            m_StopTime    = qwTime.QuadPart;
            m_ElapsedTime = qwTime.QuadPart;
            m_IsStop      = true;
        }
    }

    //---------------------------------------------------------------------------------------------
    //! @brief      0.1秒タイマーを進めます.
    //---------------------------------------------------------------------------------------------
    void Advance()
    { m_StopTime += m_TicksPerSec / 10; }

    //---------------------------------------------------------------------------------------------
    //! @brief      停止状態かどうか判定します.
    //!
    //! @retval true    停止状態.
    //! @retval false   非停止状態.
    //---------------------------------------------------------------------------------------------
    bool IsStop() const
    { return m_IsStop; }

    //---------------------------------------------------------------------------------------------
    //! @brief      システム時間を取得します.
    //!
    //! @return     システム時間を返却します.
    //---------------------------------------------------------------------------------------------
    f64 GetAbsoluteTime()
    {
        LARGE_INTEGER qwTime = { 0 };

        // 現在のカウンタを取得.
        QueryPerformanceCounter( &qwTime );

        // システム時間を算出して，返却する.
        return qwTime.QuadPart * m_InvTicksPerSec;
    }

    //---------------------------------------------------------------------------------------------
    //! @brief      相対時間を取得します.
    //!
    //! @return     相対時間を返却します.
    //---------------------------------------------------------------------------------------------
    f64 GetTime()
    {
        // 調整された現在時間を取得.
        register s64 qwTime = GetAdjustedCurrentTime();

        // 時間を算出.
        return ( qwTime - m_BaseTime ) * m_InvTicksPerSec;
    }

    //---------------------------------------------------------------------------------------------
    //! @brief      経過時間を取得します.
    //!
    //! @return     経過時間を返却します.
    //---------------------------------------------------------------------------------------------
    f64 GetElapsedTime()
    {
        // 調整された現在時間を取得.
        register s64 qwTime = GetAdjustedCurrentTime();

        // 経過時間を算出.
        register f64 elapsedTime = ( qwTime - m_ElapsedTime ) * m_InvTicksPerSec;

        // 経過時間を更新.
        m_ElapsedTime = qwTime;

        // 0以下であればランプ.
        if ( elapsedTime < 0 )
        { elapsedTime = 0.0; }

        return elapsedTime;
    }

    //---------------------------------------------------------------------------------------------
    //! @brief      時間の値を取得します.
    //!
    //! @param [out]    time           相対時間を格納する変数.
    //! @param [out]    absoluteTime   システム時間を格納する変数.
    //! @param [out]    elapsedTime    経過時間を格納する変数.
    //---------------------------------------------------------------------------------------------
    void GetValues( f64& time, f64& absoluteTime, f64& elapsedTime )
    {
        // 調整された現在時間を取得.
        s64 qwTime = GetAdjustedCurrentTime();

        // 経過時間を取得.
        register f64 diffTime = ( qwTime - m_ElapsedTime ) * m_InvTicksPerSec;

        // 経過時間を更新.
        m_ElapsedTime = qwTime;

        // 0以下であればクランプ.
        if ( diffTime < 0 )
        { diffTime = 0.0; }

        // システム時間.
        absoluteTime = qwTime * m_InvTicksPerSec;

        // 相対時間.
        time = ( qwTime - m_BaseTime ) * m_InvTicksPerSec;

        // 経過時間.
        elapsedTime = diffTime;
    }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// StopWatch class
///////////////////////////////////////////////////////////////////////////////////////////////////
class StopWatch
{
    //=============================================================================================
    // list of friend classes and methods.
    //=============================================================================================
    /* NOTHING */

private:
    //=============================================================================================
    // private variables.
    //=============================================================================================
    s64     m_TicksPerSec;          //!< 1秒あたりのタイマー刻み数.
    s64     m_StartTime;            //!< 開始時間.
    s64     m_EndTime;              //!< 停止時間.
    f64     m_InvTicksPerSec;       //!< 1タイマー刻み数当たりの秒数.

    //=============================================================================================
    // private methods.
    //=============================================================================================
    /* NOTHING */

protected:
    //=============================================================================================
    // protected variables.
    //=============================================================================================
    /* NOTHING */

    //=============================================================================================
    // protected methods.
    //=============================================================================================
    /* NOTHING */

public:
    //=============================================================================================
    // public variables.
    //=============================================================================================
    /* NOTHING */

    //=============================================================================================
    // public methods.
    //=============================================================================================

    //---------------------------------------------------------------------------------------------
    //! @brief      コンストラクタです.
    //---------------------------------------------------------------------------------------------
    StopWatch()
    : m_StartTime( 0 )
    , m_EndTime  ( 0 )
    {
        LARGE_INTEGER qwTime = { 0 };
        QueryPerformanceFrequency( &qwTime );
        m_TicksPerSec = qwTime.QuadPart;
        m_InvTicksPerSec = 1.0 / static_cast<double>( m_TicksPerSec );
    }

    //---------------------------------------------------------------------------------------------
    //! @brief      計測を開始します.
    //---------------------------------------------------------------------------------------------
    void Start()
    {
        LARGE_INTEGER qwTime = { 0 };
        QueryPerformanceCounter( &qwTime );
        m_StartTime = qwTime.QuadPart;
    }

    //---------------------------------------------------------------------------------------------
    //! @brief      計測を終了します.
    //---------------------------------------------------------------------------------------------
    void End()
    {
        LARGE_INTEGER qwTime = { 0 };
        QueryPerformanceCounter( &qwTime );
        m_EndTime = qwTime.QuadPart;
    }

    //---------------------------------------------------------------------------------------------
    //! @brief      経過時間を秒単位で取得します.
    //!
    //! @return     経過時間を秒単位で返却します.
    //---------------------------------------------------------------------------------------------
    f64 GetElapsedTimeSec()
    {
         return ( m_EndTime - m_StartTime ) * m_InvTicksPerSec;
    }

    //---------------------------------------------------------------------------------------------
    //! @brief      経過時間をミリ秒単位で取得します.
    //!
    //! @return     経過時間をミリ秒単位で返却します.
    //---------------------------------------------------------------------------------------------
    f64 GetElapsedTimeMsec()
    {
        return ( m_EndTime - m_StartTime ) * 1000.0 * m_InvTicksPerSec;
    }
};

} // namespace asdx

#endif//_ASDX_TIMER_H_