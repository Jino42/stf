#define BITS 32
//Number of buckets necessary
#define BUCK (1 << RADIX)
//Number of bits in the radix
#define RADIX 4

#define DEBUG 0

__kernel void count(
                const __global int* input,
                __global int* output,
                __local int* local_histo,
                const int pass,
                const int nkeys) {

    uint work_dim = get_work_dim();

    size_t global_id = get_global_id(0);
    size_t global_size = get_global_size(0);
    size_t local_id = get_local_id(0);
    size_t local_size = get_local_size(0);

    size_t group_id = get_group_id(0);
    size_t n_groups = get_num_groups(0);

    if (DEBUG == 1)
        printf("work_dim[%3i], global_id[%3i/%3i], local_id[%3i/%3i] groups_id[%3i/%3i]\n",
        work_dim,
        global_id, global_size,
        local_id, local_size,
        group_id, n_groups);


    int size = (nkeys / n_groups) / local_size;
    //Calculate where to start on the global array
    int start = global_id * size;

    int i;
    for(i = 0; i < BUCK; i++) {
        local_histo[i * local_size + local_id] = 0;
    }

    if (DEBUG == 1)
        printf("%i : start[%i] size[%i]\n", global_id, start, size);

    barrier(CLK_LOCAL_MEM_FENCE);

    for(i = 0; i < size; i++) {
        int key = input[i + start];
        //Extract the corresponding radix of the key
        key = ((key >> (pass * RADIX)) & (BUCK - 1));
        //Count the ocurrences in the corresponding bucket
        if (DEBUG == 1)
            printf("%i index[%i]\n", key, key * local_size + local_id);
        atomic_add(&(local_histo[key * local_size + local_id]), 1);
    }

    barrier(CLK_LOCAL_MEM_FENCE);

    for(i = 0; i < BUCK; i++) {
        //"from" references the local buckets
        int from = i * local_size + local_id;
        //"to" maps to the global buckets
        int to = i * n_groups + group_id;
        //Map the local data to its global position
        if (DEBUG == 1)
            printf("Buck[%i/%i] output[%i] = from[%i]\n", i, BUCK, local_size * to + local_id, from);
        output[local_size * to + local_id] = local_histo[from];
    }

    barrier(CLK_GLOBAL_MEM_FENCE);
}


__kernel void scan(__global int* input,
                   __global int* output,
                   __local int* local_scan,
                   __global int* block_sum,
                   int doBlockSum
                   )
{

    uint work_dim = get_work_dim();

    uint g_id = (uint) get_global_id(0);
    uint l_id = (uint) get_local_id(0);
    uint l_size = (uint) get_local_size(0);

    uint group_id = (uint) get_group_id(0);
    uint n_groups = (uint) get_num_groups(0);

    if (DEBUG == 1)
        printf("work_dim[%3i], global_id[%3i/%3i], local_id[%3i/%3i] groups_id[%3i/%3i]\n",
        work_dim,
        g_id, get_global_size(0),
        l_id, l_size,
        group_id, n_groups);

    //Store data from global to local memory to operate
    local_scan[2 * l_id] = input[2 * g_id];
    local_scan[2 * l_id + 1] = input[2 * g_id + 1];

    //UP SWEEP
    int d, offset = 1;
    for(d = l_size; d > 0; d >>= 1){
        if (DEBUG == 1 && !group_id && l_id < d)
            printf("GO | %i d[%i] > 0\n", g_id, d);
        barrier(CLK_LOCAL_MEM_FENCE);
        if(l_id < d) {
            int a = offset * (2 * l_id + 1) - 1;
            int b = offset * (2 * l_id + 2) - 1;
            if (DEBUG == 1 && !group_id)
                printf("l_id[%i] || a[%i] | b[%i] | local_scan[A]=%i | local_scan[B]=%i  |||| B+A = %i\n", g_id, a, b, local_scan[a], local_scan[b], local_scan[b] + local_scan[a]);
            local_scan[b] += local_scan[a];
        }
        offset *= 2;
    }

    if (l_id == 0) {
        //Store the full sum on last item
        if(doBlockSum != 0){
            block_sum[group_id] = local_scan[l_size * 2 - 1];
        }

        //Clear the last element
        local_scan[l_size * 2 - 1] = 0;
        if (DEBUG == 1 && !group_id) {
            printf("CLEAN 1\n");
            for (int z = 0; z < l_size * 2; z++)
                printf("%i [%3i]\n", z, local_scan[z]);
        }
    }

    //DOWN SWEEP
    for(d = 1; d < (l_size*2); d *= 2) {
        offset >>= 1;
        barrier(CLK_LOCAL_MEM_FENCE);
        if (DEBUG == 1 && !group_id && !l_id) {
            printf("CLEAN %i\n", d);
            for (int z = 0; z < l_size * 2; z++)
                printf("%i [%3i]\n", z, local_scan[z]);
        }
        if (DEBUG == 1 && !group_id && l_id < d)
            printf("GO | %i d[%i] > 0\n", g_id, d);
        barrier(CLK_LOCAL_MEM_FENCE);
        if(l_id < d) {
            int a = offset * (2 * l_id + 1) - 1;
            int b = offset * (2 * l_id + 2) - 1;
            int tmp = local_scan[a];
            local_scan[a] = local_scan[b];
            local_scan[b] += tmp;
        }
    }
    barrier(CLK_LOCAL_MEM_FENCE);

    //Write results from Local to Global memory
    output[2 * g_id]     = local_scan[2 * l_id];
    output[2 * g_id + 1] = local_scan[2 * l_id + 1];
}
__kernel void scan2(__global int* input,
                   __global int* output,
                   __local int* local_scan,
                   __global int* block_sum,
                   int doBlockSum
                   )
{

    uint work_dim = get_work_dim();

    uint g_id = (uint) get_global_id(0);
    uint l_id = (uint) get_local_id(0);
    uint l_size = (uint) get_local_size(0);

    uint group_id = (uint) get_group_id(0);
    uint n_groups = (uint) get_num_groups(0);

    if (DEBUG == 1)
        printf("work_dim[%3i], global_id[%3i/%3i], local_id[%3i/%3i] groups_id[%3i/%3i]\n",
        work_dim,
        g_id, get_global_size(0),
        l_id, l_size,
        group_id, n_groups);

    //Store data from global to local memory to operate
    local_scan[2 * l_id] = input[2 * g_id];
    local_scan[2 * l_id + 1] = input[2 * g_id + 1];


    //UP SWEEP
    int d, offset = 1;
    for(d = l_size; d > 0; d >>= 1){
        if (DEBUG == 1 && !group_id && l_id < d)
            printf("GO | %i d[%i] > 0\n", g_id, d);
        barrier(CLK_LOCAL_MEM_FENCE);
        if(l_id < d) {
            int a = offset * (2 * l_id + 1) - 1;
            int b = offset * (2 * l_id + 2) - 1;
            if (DEBUG == 1 && !group_id)
                printf("l_id[%i] || a[%i] | b[%i] | local_scan[A]=%i | local_scan[B]=%i  |||| B+A = %i\n", g_id, a, b, local_scan[a], local_scan[b], local_scan[b] + local_scan[a]);
            local_scan[b] += local_scan[a];
        }
        offset *= 2;
    }

    if (l_id == 0) {
        //Store the full sum on last item
        if(doBlockSum != 0){
            block_sum[group_id] = local_scan[l_size * 2 - 1];
        }

        //Clear the last element
        local_scan[l_size * 2 - 1] = 0;
        if (DEBUG == 1 && !group_id) {
            printf("CLEAN 1\n");
            for (int z = 0; z < l_size * 2; z++)
                printf("%i [%3i]\n", z, local_scan[z]);
        }
    }

    //DOWN SWEEP
    for(d = 1; d < (l_size*2); d *= 2) {
        offset >>= 1;
        barrier(CLK_LOCAL_MEM_FENCE);
        if (DEBUG == 1 && !group_id && !l_id) {
            printf("CLEAN %i\n", d);
            for (int z = 0; z < l_size * 2; z++)
                printf("%i [%3i]\n", z, local_scan[z]);
        }
        if (DEBUG == 1 && !group_id && l_id < d)
            printf("GO | %i d[%i] > 0\n", g_id, d);
        barrier(CLK_LOCAL_MEM_FENCE);
        if(l_id < d) {
            int a = offset * (2 * l_id + 1) - 1;
            int b = offset * (2 * l_id + 2) - 1;
            int tmp = local_scan[a];
            local_scan[a] = local_scan[b];
            local_scan[b] += tmp;
        }
    }
    barrier(CLK_LOCAL_MEM_FENCE);

    //Write results from Local to Global memory
    output[2 * g_id]     = local_scan[2 * l_id];
    output[2 * g_id + 1] = local_scan[2 * l_id + 1];
}

/** COALESCE KERNEL **/
__kernel void coalesce(__global int* scan,
                       __global int* block_sums)
{

    uint work_dim = get_work_dim();

    uint g_id = (uint) get_global_id(0);
    uint l_id = (uint) get_local_id(0);
    uint l_size = (uint) get_local_size(0);

    uint group_id = (uint) get_group_id(0);
    uint n_groups = (uint) get_num_groups(0);

    if (DEBUG == 1)
        printf("work_dim[%3i], global_id[%3i/%3i], local_id[%3i/%3i] groups_id[%3i/%3i]\n",
        work_dim,
        g_id, get_global_size(0),
        l_id, l_size,
        group_id, n_groups);

    int b = block_sums[group_id];

    //TODO: Probar pasar a memoria local
    scan[2 * g_id] += b;
    scan[2 * g_id + 1] += b;

    barrier(CLK_GLOBAL_MEM_FENCE);
}



/** REORDER KERNEL **/
__kernel void reorder(__global int* array,
                      __global int* histo, // Scan Buffer
                      __global int* output,
                      const int pass,
                      const int nkeys,
                      __local int* local_histo)
{
    uint work_dim = get_work_dim();

    uint g_id = (uint) get_global_id(0);
    uint l_id = (uint) get_local_id(0);
    uint l_size = (uint) get_local_size(0);

    uint group_id = (uint) get_group_id(0);
    uint n_groups = (uint) get_num_groups(0);

    if (DEBUG == 1)
        printf("work_dim[%3i], global_id[%3i/%3i], local_id[%3i/%3i] groups_id[%3i/%3i]\n",
        work_dim,
        g_id, get_global_size(0),
        l_id, l_size,
        group_id, n_groups);


    //Bring histo to local memory
    int i;
    for(i = 0; i < BUCK; i++){
        int to = i * n_groups + group_id;
        if (DEBUG == 1)
            printf("g_id[%i] local_histo[%i * %i + %i = %i] = histo[%i * %i + %i = %i] || %i ||\n",
            g_id,
            i, l_size, l_id, i * l_size + l_id,
            l_size, to, l_id, l_size * to + l_id,
            histo[l_size * to + l_id]);
        local_histo[i * l_size + l_id] = histo[l_size * to + l_id];
    }

    if (DEBUG == 1 && g_id == 0) {
        printf("LLocal histo : \n");
        for (int i = 0; i < l_size; i++)
            printf("%i\n", local_histo[i]);
    }
    barrier(CLK_LOCAL_MEM_FENCE);

    //Write to global memory in order
    int size = (nkeys / n_groups) / l_size;
    int start = g_id * size;

    for(i = 0; i < size; i++) {
        int item = array[i + start];
        int key = (item >> (pass * RADIX)) & (BUCK - 1);
        if (DEBUG == 1 && !group_id)
            printf("pipe l_size[%i/%i] | key[%i] | local_histo[%i]\n",
                l_id, l_size, key, key * l_size + l_id);
        int pos = local_histo[key * l_size + l_id];
        atomic_add(&(local_histo[key * l_size + l_id]), 1);

        output[pos] = item;
    }

    barrier(CLK_GLOBAL_MEM_FENCE);
}

/** CHECK ORDER KERNELS **/

__kernel void parallelcmp(const __global int* input,
                    __global int* output,
                    const int nkeys)
{
    uint g_id = (uint) get_global_id(0);
    uint l_size = (uint) get_local_size(0);
    uint n_groups = (uint) get_num_groups(0);

    //Calculate elements to process per item
    int size = (nkeys / n_groups) / l_size;
    //Calculate where to start on the global array
    int start = g_id * size;

    //Set the output to 0
    int i;
    for(i = 0; i < size; i++) {
        output[i + start] = 0;
    }

    barrier(CLK_LOCAL_MEM_FENCE);

    for(i = 0; i < size; i++) {
        if (g_id == 0 && i == 0) {
            ;
        }
        else if (input[i + start] < input[i + start - 1])
            output[i + start] = 1;
    }
    barrier(CLK_LOCAL_MEM_FENCE);
}

__kernel void reduce(__global int* input,
                     __global int* output,
                    const int nkeys)
{
    uint g_id = (uint) get_global_id(0);
    uint l_id = (uint) get_local_id(0);
    uint l_size = (uint) get_local_size(0);

    uint group_id = (uint) get_group_id(0);
    uint n_groups = (uint) get_num_groups(0);

    //UP SWEEP
    int d, offset = 1;
    for(d = l_size; d > 0; d >>= 1){
        barrier(CLK_LOCAL_MEM_FENCE);
        if(l_id < d) {
            int a = offset * (2 * l_id + 1) - 1;
            int b = offset * (2 * l_id + 2) - 1;
            input[b] += input[a];
        }
        offset *= 2;
    }

    barrier(CLK_LOCAL_MEM_FENCE);
    if(g_id == 0)
        output[0] = input[nkeys - 1];
}