#include "Looper.hpp"

#include <math.h>

namespace sain
{
    static size_t CalcIndex(float SampleRate, float LoopLenght)
    {
        return static_cast<size_t>( std::round( std::fabs( SampleRate * LoopLenght ) ) );
    }

    void Looper::SetMain( float valueLeft, float valueRight ){

    }

    void Looper::SetReturn( float valueLeft, float valueRight ){

    }

    void Looper::Process()
    {
        if ( FLAG_IS_SET( Flags, STATE_UNINITIALIZED ) )
        {
            Playhead = Buffer.begin();
            
            size_t index = CalcIndex(SampleRate, LoopLenght);

            Writehead = Buffer.from(index);

            CLEAR_FLAG(Flags, STATE_UNINITIALIZED);
        }
        if ( FLAG_IS_SET( Flags, REQ_FLIP_DIR) )
        {
            const size_t index = CalcIndex(SampleRate, LoopLenght);
            const size_t current = Playhead - Buffer.begin();

            size_t nextPos = 0;

            if ( Reverse )
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
            CLEAR_FLAG(Flags, REQ_FLIP_DIR);
        }
        
        if (Reverse)
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
}