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

int roundRNGPos(int num)
{
    if (-1000000 < num && num < 1000000) return num;
    else if (-10000000 < num && num < 10000000) return roundf(num / 10.0) * 10;
    else if (-100000000 < num && num < 100000000) return roundf(num / 100.0) * 100;
    return num;
}

uint world_seed = 0;

double search_spiral_result[3] = {};

double x_center = 0.0;
double y_center = 0.0;

double x_off = 0.0;
double y_off = 0.0;

double x_step_mult = 1.0;
double y_step_mult = 1.0;

double x_step = 1.0;
double y_step = 0.0;

uint searched_pixels = 0;

bool sampo_only = false;

const double epsilon = 0.1;

extern "C"
{

    double* search_spiral_start(uint raw_world_seed, uint newgame, double x, double y, bool find_sampo_only)
    {
        world_seed = raw_world_seed + newgame;
        searched_pixels = 0;

        x_off = 0.0;
        y_off = 0.0;

        x_step = 1.0;
        y_step = 0.0;

        //there's a better way to do this with logarithms but this is fast and also easy
        //Chest RNG components are stored with 6 decimal significant digits of precision, so we have to round according to that.
        if (abs(x) >= 1000000)       x_step_mult = 10;
        else if (abs(x) >= 10000000) x_step_mult = 100;
        else                         x_step_mult = 1;

        if (abs(y) >= 1000000)       y_step_mult = 10;
        else if (abs(y) >= 10000000) y_step_mult = 100;
        else                         y_step_mult = 1;

        x_center = x;
        y_center = y;

        search_spiral_result[0] = x;
        search_spiral_result[1] = y;

        sampo_only = find_sampo_only;

        return search_spiral_result;
    }
}

extern "C"
{

int search_spiral_step(uint max_iterations)
{
    //search for a magic pixel in a spiral
    double x_seed = search_spiral_result[0];
    double y_seed = search_spiral_result[1];

    for(int i = 0; i < max_iterations; i++)
    {
        x_seed = roundRNGPos(floor(x_center+x_off));
        y_seed = roundRNGPos(floor(y_center+y_off));
        SetRandomSeed(world_seed, x_seed, y_seed);
        bool success;
        if(sampo_only) success = Random(0, 100000) == 100000 && Random(0, 1000) != 999;
        else           success = Random(0, 100000) == 100000 && Random(0, 1000) == 999;
        searched_pixels++;
        if(success)
        {
            search_spiral_result[0] = x_seed;
            search_spiral_result[1] = y_seed;
            search_spiral_result[2] = searched_pixels;
            return true;
        }

        x_off += x_step * x_step_mult;
        y_off += y_step * y_step_mult;
        if((fabs(fabs(x_off/x_step_mult)-fabs(y_off/y_step_mult)) < epsilon && x_step <= epsilon)
           || (fabs(x_off/x_step_mult-1+y_off/y_step_mult) < epsilon) && x_step > epsilon)
        { //turn
            double temp = x_step;
            x_step = -y_step;
            y_step = temp;
        }
    }

    search_spiral_result[0] = x_seed;
    search_spiral_result[1] = y_seed;
    search_spiral_result[2] = searched_pixels;
    return false;
}

}

int64 search_portal_result[5] = {}; //parallel number, portal number, new game number, x (double), y (double)

double parallel_width = 64*512;
uint portal_world_seed;

bool portal_sampo_only = false;

double x_portal_room = (42-32)*512;
double y_portal_room = (28-14)*512;

const double portal_list[] =
{
    127-13624, 131-13824,
    366+7480 , 134-12288,
    124-17920, 257-7408 ,
    372+10240, 280+0    ,
    137-12800, 382+7008 ,
    358-4396 , 389+8504 ,
};

extern "C"
{

    int64* search_portal_start(uint raw_world_seed, int64 parallel_number, bool find_sampo_only)
    {
        portal_world_seed = raw_world_seed;

        search_portal_result[0] = parallel_number;
        search_portal_result[1] = 0;
        search_portal_result[2] = 0;
        search_portal_result[3] = 0;
        search_portal_result[4] = 0;

        portal_sampo_only = find_sampo_only;

        return search_portal_result;
    }

}

extern "C"
{

int search_portal_step(uint max_iterations)
{
    //search for a magic pixel in a portal
    int64 parallel_number = search_portal_result[0];
    double x_seed = 0;
    double y_seed = 0;

    int64 newgame = 0;
    int p = 0;
    for(int i = 0; i < max_iterations; i++)
    {

        for(newgame = 0; newgame <= 28; newgame++)
        {
            if(newgame > 0)
            {
                x_portal_room = (42-32)*512;
                y_portal_room = (28-14)*512;
                parallel_width = 64*512;
            }
            else
            {
                x_portal_room = (42-35)*512;
                y_portal_room = (28-14)*512;
                parallel_width = 70*512;
            }

            for(p = 0; p < 6; p++)
            {
                x_seed = floor(x_portal_room+parallel_number*parallel_width+portal_list[2*p+0]);
                y_seed = floor(y_portal_room+portal_list[2*p+1]);
                uint world_seed = portal_world_seed+newgame;
                SetRandomSeed(world_seed, x_seed, y_seed);
                bool success = false;
                //NOTE: for some reason this does not work, but I'm going to remove it anyway so not worth debugging
                if(portal_sampo_only) success = Random(0, 10) == 10 && Random(0, 1000) != 999;
                else                  success = Random(0, 100000) == 100000 && Random(0, 1000) == 999;
                if(success)
                {
                    search_portal_result[0] = parallel_number;
                    search_portal_result[1] = p;
                    search_portal_result[2] = newgame;
                    search_portal_result[3] = *((int64*) &x_seed);
                    search_portal_result[4] = *((int64*) &y_seed);
                    return true;
                }
            }
        }
        if(parallel_number > 0) parallel_number =  -parallel_number;
        else                    parallel_number = 1-parallel_number;
    }

    search_portal_result[0] = parallel_number;
    search_portal_result[1] = p;
    search_portal_result[2] = newgame;
    search_portal_result[3] = *((int64*) &x_seed);
    search_portal_result[4] = *((int64*) &y_seed);
    return false;
}

}
