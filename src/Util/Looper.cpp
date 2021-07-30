#include "Looper.hpp"

#include <cmath>

namespace sain
{
    static size_t CalcIndex(float SampleRate, float LoopLenght)
    {
        return static_cast<size_t>( std::round( std::fabs( SampleRate * LoopLenght ) ) );
    }

    void Looper::WriteSample( float valueT1, float valueT2 )
    {
        WriteSample( { valueT1, valueT2 } );
    }

    void Looper::WriteSample( Sample value )
    {
        *Writehead = value;
    }

    void Looper::Process()
    {
        if ( FLAG_IS_SET( Flags, LooperFlags::STATE_UNINITIALIZED ) )
        {
            Playhead = Buffer.begin();
            
            size_t index = CalcIndex(SampleRate, LoopLenght);

            Writehead = Buffer.from(index);

            CLEAR_FLAG(Flags, LooperFlags::STATE_UNINITIALIZED);
        }
        if ( FLAG_IS_SET( Flags, LooperFlags::REQ_FLIP_DIR) )
        {
            const size_t index = CalcIndex(SampleRate, LoopLenght);
            const size_t current = Playhead - Buffer.begin();

            size_t nextPos = 0;

            if ( ReverseState )
            {
                if (index > current)
                {
                    nextPos = buffer_size - (index - current);
                }else
                {
                    nextPos = current - index;
                }
            } 
            else
            {   
                nextPos = current + index;
                if( nextPos > buffer_size ){
                    nextPos -= buffer_size;
                } 
            }
            
            Writehead = Buffer.from(nextPos);
            CLEAR_FLAG(Flags, LooperFlags::REQ_FLIP_DIR);
        }
        
        if (ReverseState)
        {
            --Playhead;
            --Writehead;
        }
        else
        {
            ++Playhead;
            ++Writehead;
        }
    }

    void Looper::SetSampleRate(float value)
    {
		SampleRate = value;

        Playhead = Buffer.begin();
            
        size_t index = CalcIndex(SampleRate, LoopLenght);

        Writehead = Buffer.from(index);

		CLEAR_FLAG(Flags, LooperFlags::STATE_UNINITIALIZED);
	}
}