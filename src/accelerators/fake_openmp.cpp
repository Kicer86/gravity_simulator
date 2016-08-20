
extern "C"
{

    int omp_get_max_threads(void)
    {
        return 1;
    }

    int omp_get_thread_num(void)
    {
        return 0;
    }

}
