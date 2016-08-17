
int main(void)
{
#ifdef __AVX__
    return 1;
#else
    return 0;
#endif
}
