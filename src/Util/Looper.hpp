#pragma once
#include "Buffer.hpp"
#include "Bitflag.hpp"

namespace sain
{
	constexpr size_t buffer_size = 1ull << 21ull;

	enum class LooperFlags : int8_t
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
            Sample() = default;

            Sample(const float& valueT1, const float& valueT2) :
                T1(valueT1),
                T2(valueT2)
            {}

            Sample(const Sample& value) :
                T1(value.T1),
                T2(value.T2)
            {}

            Sample& operator=(const Sample& rhs)
            {
                this->T1 = rhs.T1;
                this->T2 = rhs.T2;
                return *this;
            }

            Sample& operator+=(const Sample& rhs)
            {
                this->T1 += rhs.T1;
                this->T2 += rhs.T2;
                return *this;
            }
		};

    private:
		RingBuffer<Sample, buffer_size> Buffer;

		typedef typename RingBuffer<Sample, buffer_size>::iterator iterator;

		iterator Playhead;
		iterator Writehead;

        const float LoopLenght;
		float SampleRate	= 0.0f;
		LooperFlags Flags = LooperFlags::STATE_UNINITIALIZED;

		bool ReverseState = false;

	 public:
		Looper( ) :
            LoopLenght(8.0f)
        {}

        Looper( const float lenght ) :
            LoopLenght(lenght)
        {}

		void WriteSample( float valueLeft, float valueRight );
		void WriteSample( Sample value );

		inline Sample ReadSample( )
		{
			return *Playhead;
		}

		inline void Reverse()
		{
			ReverseState = !ReverseState;
			SET_FLAG(Flags, LooperFlags::REQ_FLIP_DIR);
		}

		void SetSampleRate( float value );
		

		void Process( );
	};

}	 // namespace sain