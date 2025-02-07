* single model mesh batching (prototype for other types of batching)
* shader generation
* performance + logging
* shader quality tweeks
* weather system, wind, vertex pivot animation
* LOD?
* post processing effects refactoring, fog, soft particles, soft shadows?, screen space shadows resolution tweeks
* indirect draw, compute shaders
* light culling
* other optimizations
* shadows refactoring


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


    void draw(Instance instance) {
        
        instance.mesh

    }


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
