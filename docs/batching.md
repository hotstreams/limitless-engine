* features
0. gpu & cpu counters, logging
1. vertex pivot animation, wind
2. billboards
2. weather system
3. post processing effects (fog, soft particles, soft shadows?, screen space shadows resolution tweeks)
   Multiple tone mappers: generic (customizable), ACES, filmic, etc.
   Color and tone management: luminance scaling, gamut mapping
   Color grading: exposure, night adaptation, white balance, channel mixer, shadows/mid-tones/highlights, ASC CDL, contrast, saturation, etc.
4. shadows refactoring
5. height fog & volumetric fog, color grading and more

* optimizations
1. single model mesh batching + shader generation 
2. texture compression 
3. gbuffer reformatting
4. LOD
5. indirect draw
6. compute shaders (for skeletal & wind animation, frustum culling, light culling)
7. combine all geometry in one buffer (global vao, vbo?)
8. possibly combine another things in ssbo/ubo to reduce state binding changes
9. possible optimizations after profiling 
10. shader quality (switch different shaders on different LODs based on distance) (Arguably useful, because cost of switching could be more than shader computation)

https://www.reddit.com/r/unrealengine/comments/192pj36/how_to_better_optimize_forest_for_a_video_game/

static mesh batching in one model
static batching several models?
static instance batching?
dynamic instance batching?

glDrawElementsBaseVertex();
glMultiDrawElements();
glMultiDrawElementsBaseVertex();

// 3.3


// 4.2
glDrawElementsInstancedBaseVertexBaseInstance();


GEOMETRY

vertex stream
    descriptor: array of attributes
    
    array of attributes

    void drawArrays

indexed vertex stream

    array of indices

    void drawElements

batched vertex stream

    all in one array

    void drawelements

batched indexed vertex stream

3.3
    all in one array

    add mesh id to vertex attribute

    void drawelements

    void drawelementsinstanced

4.2 can use direct/indirect draw

    direct

    glDrawElementsInstancedBaseVertexBaseInstance

    indirect

    glmultidrawindirect

MATERIALS

    batched material


Renderer 

    VertexStream::builder
        .attribute
        .attribute

        .index_buffer
        .vertex_buffer

        .pointer_to_data
        .pointer_to_indices

        .batch(vertex_stream)
        .batch(vertex_stream)

        .build()


    Mesh
        vertex stream

    Model
        meshes info

        meshes -> batched mesh
        materials -> batched material


    Instance
        MeshInstances


    BatchMeshInstance - ?


    Instance
        ModelInstance
            MeshInstances - storage for data

            SkeletalInstance

            BatchedInstance
                BatchedMeshInstance

                update - propogate changes from MeshInstances to BatchedMeshInstance
                    renderer will deside how to render mesh instances (possibly renderer batched and some not batched)

            IndirectInstance

        InstancedInstance - common interface for instanced instance
            Instances - arrays of instances

            array of instance data

            InstancedSkeletalInstance
                array of skeletal data

            InstancedBatchedInstance

            InstancedDecalInstance

        DecalInstance

        TerrainInstance

        EffectInstance - ?

    Renderer
