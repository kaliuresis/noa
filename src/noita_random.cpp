#include <stdint.h>
#include <math.h>

typedef unsigned int uint;

typedef uint8_t byte;
typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;
typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;
typedef uint8 bool8;

#define ever (;;)

#define len(array) sizeof(array)/sizeof((array)[0])

double random_seed = 0;

extern "C"
{

int Random(int a, int b)

{
    int iVar1;

    iVar1 = (int) random_seed * 0x41a7 + ((int) random_seed / 0x1f31d) * -0x7fffffff;
    if (iVar1 < 1) {
        iVar1 = iVar1 + 0x7fffffff;
    }
    random_seed = (double)iVar1;
    return a - (int)((double)((b - a) + 1) * (double)iVar1 * -4.656612875e-10);
}

}

uint64 SetRandomSeedHelper(double r)
{
    uint64 e = *(uint64*)&r;
    if(((e >> 0x20 & 0x7fffffff) < 0x7FF00000)
       && (-9.223372036854776e+18 <= r) && (r < 9.223372036854776e+18))
    {
        //should be same as e &= ~(1<<63); which should also just clears the sign bit,
        //or maybe it does nothing,
        //but want to keep it as close to the assembly as possible for now
        e <<= 1;
        e >>= 1;
        double s = *(double*) &e;
        uint64 i = 0;
        if(s != 0.0)
        {
            uint64 f = (((uint64) e) & 0xfffffffffffff) | 0x0010000000000000;
            uint64 g = 0x433 - ((uint64) e >> 0x34);
            uint64 h = f >> g;

            int j = -(uint)(0x433 < ((e >> 0x20)&0xFFFFFFFF) >> 0x14);
            i = (uint64) j<<0x20 | j;
            i = ~i & h | f << (((uint64) s >> 0x34) - 0x433) & i;
            i = ~-(uint64)(r == s) & -i | i & -(uint64)(r == s);
            // error handling, whatever
            // f = f ^
            // if((int) g > 0 && f )
        }
        return i & 0xFFFFFFFF;
    }

    //error!
    uint64 error_ret_val = 0x8000000000000000;
    return *(double*) &error_ret_val;
}

uint SetRandomSeedHelper2(int param_1,int param_2,uint param_3)
{
    uint uVar1;
    uint uVar2;
    uint uVar3;

    uVar2 = (param_1 - param_2) - param_3 ^ param_3 >> 0xd;
    uVar1 = (param_2 - uVar2) - param_3 ^ uVar2 << 8;
    uVar3 = (param_3 - uVar2) - uVar1 ^ uVar1 >> 0xd;
    uVar2 = (uVar2 - uVar1) - uVar3 ^ uVar3 >> 0xc;
    uVar1 = (uVar1 - uVar2) - uVar3 ^ uVar2 << 0x10;
    uVar3 = (uVar3 - uVar2) - uVar1 ^ uVar1 >> 5;
    uVar2 = (uVar2 - uVar1) - uVar3 ^ uVar3 >> 3;
    uVar1 = (uVar1 - uVar2) - uVar3 ^ uVar2 << 10;
    return (uVar3 - uVar2) - uVar1 ^ uVar1 >> 0xf;
}


extern "C"
{

void SetRandomSeed(uint world_seed, double x, double y)
{
    uint a = world_seed ^ 0x93262e6f;
    uint b = a & 0xfff;
    uint c = (a >> 0xc) & 0xfff;

    double x_ = x+b;

    double y_ = y+c;

    double r = x_*134217727.0;
    uint64 e = SetRandomSeedHelper(r);

    uint64 _x = (*(uint64*) &x_ & 0x7FFFFFFFFFFFFFFF);
    uint64 _y = (*(uint64*) &y_ & 0x7FFFFFFFFFFFFFFF);
    if(102400.0 <= *((double*) &_y) || *((double*) &_x) <= 1.0)
    {
        r = y_*134217727.0;
    }
    else
    {
        double y__ = y_*3483.328;
        double t = e;
        y__ += t;
        y_ *= y__;
        r = y_;
    }

    uint64 f = SetRandomSeedHelper(r);

    uint g = SetRandomSeedHelper2(e, f, world_seed);
    double s = g;
    s /= 4294967295.0;
    s *= 2147483639.0;
    s += 1.0;

    if(2147483647.0 <= s) {
        s = s*0.5;
    }
    random_seed = s;

    Random(0, 0);

    uint h = world_seed&3;
    while(h)
    {
        Random(0, 0);
        h--;
    }
}

}

uint world_seed = 0;

double search_result[2] = {};

double x_center = 0.0;
double y_center = 0.0;

double x_off = 0.0;
double y_off = 0.0;

double x_step = 1.0;
double y_step = 0.0;

const double epsilon = 0.1;

extern "C"
{

double* search_spiral_start(uint raw_world_seed, uint newgame, double x, double y)
{
    world_seed = raw_world_seed + newgame;

    x_off = 0.0;
    y_off = 0.0;

    x_step = 1.0;
    y_step = 0.0;

    x_center = x;
    y_center = y;

    search_result[0] = x;
    search_result[1] = y;

    return search_result;
}

}

extern "C"
{

int search_spiral_step(uint max_iterations)
{
    //search for a magic pixel in a spiral
    double x_seed = search_result[0];
    double y_seed = search_result[1];

    for(int i = 0; i < max_iterations; i++)
    {
        x_seed = floor(x_center+x_off);
        y_seed = floor(y_center+y_off);
        SetRandomSeed(world_seed, x_seed, y_seed);
        if(Random(0, 100000) == 100000 && Random(0, 1000) == 999)
        {
            search_result[0] = x_seed;
            search_result[1] = y_seed;
            return true;
        }

        x_off += x_step;
        y_off += y_step;
        if((fabs(fabs(x_off)-fabs(y_off)) < epsilon && x_step <= epsilon)
           || (fabs(x_off-1.0+y_off) < epsilon) && x_step > epsilon)
        { //turn
            double temp = x_step;
            x_step = -y_step;
            y_step = temp;
        }
    }

    search_result[0] = x_seed;
    search_result[1] = y_seed;
    return false;
}

}
