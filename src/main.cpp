
#include <fenv.h>

#include <QApplication>

#include "main_window.hpp"
#include "simulation_engine.hpp"

/*
Copyright (c) 2012, Magazyn Programista
Copyright (c) 2012, Programmer's Magazine

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

// Compute1.cpp : Defines the entry point for the console application.
//

//#include "stdafx.h"


#include <vector>
#include <algorithm>
#include <iostream>

#include <boost/compute/core.hpp>
#include <boost/compute/algorithm/copy.hpp>
#include <boost/compute/algorithm/transform.hpp>
#include <boost/compute/algorithm/accumulate.hpp>
#include <boost/compute/algorithm/for_each.hpp>
#include <boost/compute/container/vector.hpp>
#include <boost/compute/functional/math.hpp>
#include <boost/compute/functional/operator.hpp>


namespace cpt = boost::compute;
using namespace std;

void test1()
{
    cpt::device device = cpt::system::default_device();

    //cpt::device device_sel = cpt::system::find_device("GeForce GTX 970");

    cout << "Nazwa domyślnego urządzenia: " << device.name() << endl;
    //cout << "Nazwa wskazanego urządzenia: " << device_sel.name() << endl;

    vector<cpt::platform> platforms = cpt::system::platforms();

    for (size_t i = 0; i < platforms.size(); i++){
        const cpt::platform &platform = platforms[i];

        cout << "Platforma '" << platform.name() << "'" << endl;

        vector<cpt::device> devices = platform.devices();

        for (size_t j = 0; j < devices.size(); j++){
            const cpt::device &device = devices[j];

            string type;
            if (device.type() & cpt::device::gpu)
                type = "urządzenie GPU";
            else if (device.type() & cpt::device::cpu)
                type = "urządzenie CPU";
            else if (device.type() & cpt::device::accelerator)
                type = "urządzenie typu accelerator";
            else
                type = "nieznany typ urządzenia";

            cout << "  " << type << ": " << device.name() << endl;
        }
    }
}

void test2()
{
    cpt::device device = cpt::system::default_device();
    cpt::context context(device);
    cpt::command_queue queue(context, device);

    float host_v[] = { 1.2f, 2.3f, 2.2f, 7.0f, 5.25f };

    cpt::vector<float> dev_v(5, context);
    cpt::copy( host_v, host_v + 5, dev_v.begin(), queue );

    cpt::transform(
        dev_v.begin(),
                   dev_v.end(),
                   dev_v.begin(),
                   cpt::sqrt<float>(),
                   queue );

    std::vector<float> rslt_host_v(5);

    cpt::copy(
        dev_v.begin(), dev_v.end(), rslt_host_v.begin(), queue );

    cout << endl;
    for ( auto v : rslt_host_v)
        cout << v << " ";
    cout << endl;
}

void test3()
{
    //cpt::device device = cpt::system::default_device();
    cpt::device device = cpt::system::find_device("GeForce GTX 970");

    cpt::context context(device);
    cpt::command_queue queue(context, device);

    cout << "Nazwa domylnego urzšdzenia: " << device.name() << endl;


    float host_v[] = { 1.2f, 2.3f, 2.2f, 7.0f, 5.25f };

    cpt::vector<float> dev_v(5, context);
    //cpt::vector<float> dev_output(5, context);

    cpt::copy(host_v, host_v + 5, dev_v.begin(), queue);

    /*
     *    cpt::function<float(float)> add_ten =
     *        boost::compute::make_function_from_source<float(float)>(
     *        "add_ten",
     *        "float add_ten(float x) { return x + 10.0f; }"
     *        );
     */

    BOOST_COMPUTE_FUNCTION(float, add_ten, (float x),
                           {
                               return x + 10.f;
                           });


    cpt::transform(dev_v.begin(), dev_v.end(), dev_v.begin(), add_ten, queue);


    float rslt = 0.0f;

    cpt::reduce(dev_v.begin(), dev_v.end(), &rslt, cpt::plus<float>(), queue);

    cout << "rslt = " << rslt << endl;
}

void test4()
{
    const int N = 4;

    #if 0
    const char kernel_source[] =
    "__kernel void add_vector(__global const float *a,"
    "                  __global const float *b,"
    "                  __global float *c)"
    "{"
    "    const uint i = get_global_id(0);"
    "    c[i] = a[i] + b[i];"
    "}";
    #endif

    const char kernel_source[] = BOOST_COMPUTE_STRINGIZE_SOURCE(
        __kernel void add_vector(__global const float *a,
                                 __global const float *b,
                                 __global float *c)
        {
            const uint i = get_global_id(0);
            c[i] = a[i] + b[i];
        };
    ); // end of BOOST_COMPUTE_STRINGIZE_SOURCE

    cpt::device device = cpt::system::find_device("GeForce GTX 970");

    cpt::context context(device);
    cpt::command_queue queue(context, device);

    float host_a[] = { 1, 2, 3, 4 };
    float host_b[] = { 4, 3, 2, 1 };

    float host_c[] = { 0, 0, 0, 0 };

    cpt::buffer dev_a(context, N * sizeof(float));
    cpt::buffer dev_b(context, N * sizeof(float));
    cpt::buffer dev_c(context, N * sizeof(float));

    queue.enqueue_write_buffer(dev_a, 0, N * sizeof(float), host_a);
    queue.enqueue_write_buffer(dev_b, 0, N * sizeof(float), host_b);

    cpt::program program =
    cpt::program::create_with_source(kernel_source, context);

    program.build();

    cpt::kernel kernel(program, "add_vector");

    kernel.set_arg(0, dev_a);
    kernel.set_arg(1, dev_b);
    kernel.set_arg(2, dev_c);


    queue.enqueue_1d_range_kernel(kernel, 0, N, 0);

    queue.enqueue_read_buffer(dev_c, 0, N * sizeof(float), host_c);

    for (auto v : host_c)
        cout << v << " ";
    cout << endl;
}

unsigned int hsx = 1024;
unsigned int hsy = 512;

unsigned char* image_input = NULL;
unsigned char* image_output = NULL;

void* load_grey_data(char *fname, int sx, int sy)
{

    FILE *f;
    unsigned char *image_data;

    image_data = (unsigned char *)malloc(sx * sy);

    f = fopen(fname, "r");
    fread(image_data, 1, sx*sy, f);
    fclose(f);


    return (void*)image_data;
}

void save_grey_data(char *fname, void *source_data, int sx, int sy)
{
    FILE *f;

    f = fopen(fname, "wb");
    fwrite(source_data, 1, sx*sy, f);
    fclose(f);
}

void test5()
{
    cpt::device device = cpt::system::find_device("GeForce GTX 970");

    cpt::context context(device);
    cpt::command_queue queue(context, device);

    const char kernel_sobel_source[] = BOOST_COMPUTE_STRINGIZE_SOURCE(
        __kernel void sobel_grey( __global uchar* input, __global uchar* output, unsigned int sx, unsigned int sy)
        {
            int ix = get_global_id(0);
            int iy = get_global_id(1);

            if ( ix == 0 || ix == (sx - 1) )
            {
                output[ iy * sx ] = 255;
                output[ (( iy + 1) * sx) - 1 ] = 255;
            }

            if ( iy == 1 || iy == (sy - 2) )
            {
                output[ ix ] = 255;
                output[ ix + ((sy-1) * sx) ] = 255;
            }
            if( (iy>0 && iy<(sy-1)) && (ix>0 && ix<(sx-1)) )
            {
                int sum_x=0, sum_y=0, sum=0;

                sum_x += (int)( input[ ix - 1 + (iy - 1)*sx] * -1);
                sum_x += (int)( input[ ix     + (iy - 1)*sx] * -2);
                sum_x += (int)( input[ ix + 1 + (iy - 1)*sx] * -1);

                sum_x += (int)( input[ ix - 1 + (iy + 1)*sx] * 1);
                sum_x += (int)( input[ ix     + (iy + 1)*sx] * 2);
                sum_x += (int)( input[ ix + 1 + (iy + 1)*sx] * 1);

                sum_x = min(255, max( 0, sum_x ));

                sum_y += (int)( input[ ix - 1 + (iy - 1)*sx] * 1);
                sum_y += (int)( input[ ix + 1 + (iy - 1)*sx] * -1);

                sum_y += (int)( input[ ix - 1 + (iy)*sx] * 2);
                sum_y += (int)( input[ ix + 1 + (iy)*sx] * -2);

                sum_y+= (int)( input[ ix - 1 + (iy + 1)*sx] * 1);
                sum_y+= (int)( input[ ix + 1 + (iy + 1)*sx] * -1);

                sum_y = min(255, max( 0, sum_y ));

                sum = abs(sum_x) + abs(sum_y);

                output[ ix + (iy * sx)] = 255 - (unsigned char)(sum);
            }
        }
    ); // end of BOOST_COMPUTE_STRINGIZE_SOURCE


    image_input = (unsigned char*)load_grey_data("test-grey-2.raw", hsx, hsy);
    image_output = (unsigned char*)malloc( hsx * hsy );

    cpt::buffer dev_input(context, hsx * hsy);
    cpt::buffer dev_output(context, hsx * hsy);

    queue.enqueue_write_buffer(dev_input, 0, hsx * hsy, image_input);

    cpt::program program =
    cpt::program::create_with_source(kernel_sobel_source, context);

    program.build();

    cpt::kernel kernel(program, "sobel_grey");

    kernel.set_arg(0, dev_input);
    kernel.set_arg(1, dev_output);
    kernel.set_arg(2, hsx);
    kernel.set_arg(3, hsy);

    const size_t global_work_offset[] = { 0, 0 };
    const size_t global_work_size[] = { size_t(hsx), size_t(hsy) };
    const size_t local_work_size[] = { size_t(16), size_t(16) };

    queue.enqueue_nd_range_kernel(kernel, 2, global_work_offset, global_work_size, local_work_size);

    queue.enqueue_read_buffer(dev_output, 0, hsx * hsy, image_output);

    save_grey_data("test-sobel-grey-2.raw", image_output, hsx, hsy);
}


int main(int argc, char** argv)
{
    feenableexcept(FE_INVALID | FE_OVERFLOW);

    test1();

    QApplication app(argc, argv);
    MainWindow window;
    window.show();

    return app.exec();
}
