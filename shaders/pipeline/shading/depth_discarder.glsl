#include "./fragment.glsl"

/*
    this function is used when
    fragment shader outputs only depth values
    but material can discard some fragments based on properties
    so we invoke this code
*/

void discardForDepth() {
    FragmentData data = initializeFragmentData();
    customFragment(data);
    getFragmentBaseColor(data);
}
