void kernel simple_add(global const int* A, global const int* B, global int* C){
	C[get_global_id(0)]=A[get_global_id(0)]+B[get_global_id(0)];
}

__kernel void reduce(__global float *a,
                     __global float *r,
                     __local float *b)
{
    uint gid = get_global_id(0);
    uint wid = get_group_id(0);
    uint lid = get_local_id(0);
    uint gs = get_local_size(0);
 
    b[lid] = a[gid];
    barrier(CLK_LOCAL_MEM_FENCE);
 
    for(uint s = gs/2; s > 0; s >>= 1) {
        if(lid < s) {
          b[lid] += b[lid+s];
        }
        barrier(CLK_LOCAL_MEM_FENCE);
    }
    if(lid == 0) r[wid] = b[lid];
}

__kernel void par_sum(__global int* buffer, __global int *scratch, __global int *len, __global int *result) {
	int global_index;
	int accumulator;
	size_t global_size;
	int local_len;

	barrier(CLK_GLOBAL_MEM_FENCE);
	global_index = get_global_id(0);
	accumulator = 0;
	global_size = get_global_size(0);
	local_len = *len;

	// Loop sequentially over chunks of input vector
	while (global_index < *len) {
		int element = buffer[global_index];
		accumulator += element;
		global_index += global_size;
	}
	// if(get_global_id(0) % 4747 == 0) printf("%d %d %d\n", get_global_id(0), get_global_size(0), accumulator);
	// Perform parallel reduction
	global_index = get_global_id(0);
	scratch[global_index] = accumulator;
	barrier(CLK_GLOBAL_MEM_FENCE);
	for (int last_size = global_size; last_size > 1; last_size = (last_size + 1)/2) {
		int half_block = (last_size + 1) / 2;
		if (global_index < last_size/2) {
			int other = scratch[global_index + half_block];
			scratch[global_index] += other;
		}
		barrier(CLK_GLOBAL_MEM_FENCE);
	}
	
	if (global_index == 0) {
		*result = scratch[0];
	}
}
