#include "ParticleData.hl"

//__kernel void getNbParticleActive(
__kernel void getNbParticleActiveSafe(
    __global ParticleData const *data,
    __global float *bufferDist,
    __global int *nbParticleActive,
    int nbParticleMax)
{
    int id = get_global_id(0);
    __global ParticleData *particle = &data[id];

    int s = 0;
    int c = nbParticleMax / 2;

/*
    while (c) {
        if (particleIsActive(&data[s + c - 1]))
        {
            s += c;
            c = round(c / 2.0f);
        }
        else
        {
            c /= 2;
        }
    }
    *nbParticleActive = s;
    */

    int left = 0;
    int right = nbParticleMax;
    while (left <= right)
    {
        int m = floor((float)(left + right) / 2.0f);
        if (particleIsActive(&data[m]))
            left = m + 1;
        else
            right = m - 1;
    }
    for (int i = left + 1; i < nbParticleMax; i++)
    {
        if (particleIsActive(&data[i]))
            left++;
    }

    *nbParticleActive = left;
}

//__kernel void getNbParticleActiveSafe(
__kernel void getNbParticleActiveSafe2(
    __global ParticleData const *data,
    __global float *bufferDist,
    __global int *nbParticleActive,
    int nbParticleMax)
{
    int id = get_global_id(0);
    __global ParticleData *particle = &data[id];

    int i = 0;
    int s = 0;
    while (i < nbParticleMax)
    {
        if (particleIsActive(&data[i]))
            s++;
        i++;
    }
    *nbParticleActive = s;
}

__kernel void calculateDistanceBetweenParticleAndCamera(
    __global ParticleData const *data,
    __global float *bufferDist,
    float3 cameraPosition)
{
    int id = get_global_id(0);
    __global ParticleData *particle = &data[id];

    bufferDist[id] = fabs((particle->position.x - cameraPosition.x) + (particle->position.y - cameraPosition.y) + (particle->position.z - cameraPosition.z));
    if (!particleIsActive(particle))
        bufferDist[id] = -10.f;
}

__kernel void ParallelSelection(
    __global ParticleData *data,
    __global ParticleSpriteData *spriteData,
    __global float *bufferDist,
    float3 cameraPosition)
{
    int i = get_global_id(0); // current thread
    int n = get_global_size(0); // input size
    ParticleData iDataParticle = data[i];
    ParticleSpriteData iDataSprite = spriteData[i];

    float iKey = bufferDist[i];
    int pos = 0;
    for (int j = 0; j < n; j++)
    {
        if ((bufferDist[j] > iKey) || (bufferDist[j] == iKey && j < i))
            pos += 1;
    }
    data[pos] = iDataParticle;
    spriteData[pos] = iDataSprite;
}

/*
__kernel void ParallelSelection_Blocks(
                    __global ParticleData *data,
                    __local uint *aux,
                    float3 cameraPosition,
                    int blockFactor)
{
  int i = get_global_id(0); // current thread
  int n = get_global_size(0); // input size
  int wg = get_local_size(0); // workgroup size
  ParticleData iData = data[i]; // input record for current thread
  uint iKey = keyValue(iData, cameraPosition); // input key for current thread
  int blockSize = blockFactor * wg; // block size

  // Compute position of iKey in output
  int pos = 0;
  // Loop on blocks of size BLOCKSIZE keys (BLOCKSIZE must divide N)
  for (int j=0;j<n;j+=blockSize)
  {
    // Load BLOCKSIZE keys using all threads (blockFactor values per thread)
    barrier(CLK_LOCAL_MEM_FENCE);
    for (int index=get_local_id(0);index<blockSize;index+=wg)
      aux[index] = keyValue(data[j+index], cameraPosition);
    barrier(CLK_LOCAL_MEM_FENCE);

    // Loop on all values in AUX
    for (int index=0;index<blockSize;index++)
    {
     uint jKey = aux[index]; // broadcasted, local memory
      bool smaller = (jKey < iKey) || ( jKey == iKey && (j+index) < i ); // data[j] < data[i] ?
      pos += (smaller)?1:0;
    }
  }
  data[pos] = iData;
}

__kernel void ParallelMerge_Local(__global ParticleData *data, __local ParticleData *aux, float3 cameraPosition)
{
  int i = get_local_id(0); // index in workgroup
  int wg = get_local_size(0); // workgroup size = block size, power of 2

  // Move IN to block start
  int offset = get_group_id(0) * wg;
  data += offset;

  // Load block in AUX[WG]
  aux[i] = data[i];
  barrier(CLK_LOCAL_MEM_FENCE); // make sure AUX is entirely up to date

  // Now we will merge sub-sequences of length 1,2,...,WG/2
  for (int length=1;length<wg;length<<=1)
  {
    ParticleData iData = aux[i];
    float iKey = keyValue(iData, cameraPosition);
    int ii = i & (length-1);  // index in our sequence in 0..length-1
    int sibling = (i - ii) ^ length; // beginning of the sibling sequence
    int pos = 0;
    for (int inc=length;inc>0;inc>>=1) // increment for dichotomic search
    {
      int j = sibling+pos+inc-1;
      float jKey = keyValue(aux[j], cameraPosition);
      bool smaller = (jKey < iKey) || ( jKey == iKey && j < i );
      pos += (smaller)?inc:0;
      pos = min(pos,length);
    }
    int bits = 2*length-1; // mask for destination
    int dest = ((ii + pos) & bits) | (i & ~bits); // destination index in merged sequence
    barrier(CLK_LOCAL_MEM_FENCE);
    aux[dest] = iData;
    barrier(CLK_LOCAL_MEM_FENCE);
  }

  // Write output
  data[i] = aux[i];
}
*/