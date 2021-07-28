#pragma once
#include "Buffer.hpp"
#include "Bitflag.hpp"

namespace sain
{
	constexpr size_t buffer_size = 1ull << 21ull;

	enum LooperFlags : int8_t
	{
		STATE_OK            = 0b0,
		STATE_UNINITIALIZED = 1 << 0,
		REQ_FLIP_DIR        = 1 << 1
	};

	DEF_FLAG_OR_OP( LooperFlags, int8_t )
	DEF_FLAG_AND_OP( LooperFlags, int8_t )
    DEF_FLAG_ASSIGN_OR_OP( LooperFlags, int8_t)
    DEF_FLAG_ASSIGN_AND_OP( LooperFlags, int8_t)
    DEF_FLAG_NOT_OP( LooperFlags, int8_t)

	class Looper
	{
    public:
		struct Sample
		{
			float T1 = 0.0f, T2 = 0.0f;
		};

    private:
		RingBuffer<Sample, buffer_size> Buffer;

		typedef typename RingBuffer<Sample, buffer_size>::iterator iterator;

		iterator Playhead;
		iterator Writehead;

        const float LoopLenght;
		float SampleRate	= 0.0f;
		LooperFlags Flags = STATE_UNINITIALIZED;

		bool Reverse = false;

	 public:
		Looper( ) :
            LoopLenght(8.0f)
        {}

        Looper( const float lenght ) :
            LoopLenght(lenght)
        {
            
        }

		void SetMain( float valueLeft, float valueRight );
		void SetReturn( float valueLeft, float valueRight );

		inline Sample GetMain( )
		{
			return *Playhead;
		}

		inline void SetReverse( bool value )
		{
			Reverse = value;
			SET_FLAG(Flags, REQ_FLIP_DIR);
		}

		inline void SetSampleRate( float value )
		{
			SampleRate = value;
			SET_FLAG(Flags, STATE_UNINITIALIZED);
		}

		void Process( );
	};

}	 // namespace sain