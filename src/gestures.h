#include "gl_env.h"
#include "skeletal_mesh.h"
#include <glm/gtc/matrix_transform.hpp>

#define M_PI (3.1415926535897932)

/**********************************************************************************\
       *
       * To animate fingers, modify modifier["HAND_SECTION"] each frame,
       * where HAND_SECTION can only be one of the bone names in the Hand's Hierarchy.
       *
       * A virtual hand's structure is like this: (slightly DIFFERENT from the real world)
       *    5432 1
       *    ....        1 = thumb           . = fingertip
       *    |||| .      2 = index finger    | = distal phalange
       *    $$$$ |      3 = middle finger   $ = intermediate phalange
       *    #### $      4 = ring finger     # = proximal phalange
       *    OOOO#       5 = pinky           O = metacarpals
       *     OOO
       * (Hand in the real world -> https://en.wikipedia.org/wiki/Hand)
       *
       * From the structure we can infer the Hand's Hierarchy:
       *	- metacarpals
       *		- thumb_proximal_phalange
       *			- thumb_intermediate_phalange
       *				- thumb_distal_phalange
       *					- thumb_fingertip
       *		- index_proximal_phalange
       *			- index_intermediate_phalange
       *				- index_distal_phalange
       *					- index_fingertip
       *		- middle_proximal_phalange
       *			- middle_intermediate_phalange
       *				- middle_distal_phalange
       *					- middle_fingertip
       *		- ring_proximal_phalange
       *			- ring_intermediate_phalange
       *				- ring_distal_phalange
       *					- ring_fingertip
       *		- pinky_proximal_phalange
       *			- pinky_intermediate_phalange
       *				- pinky_distal_phalange
       *					- pinky_fingertip
       *
       * Notice that modifier["HAND_SECTION"] is a local transformation matrix,
       * where (1, 0, 0) is the bone's direction, and apparently (0, 1, 0) / (0, 0, 1)
       * is perpendicular to the bone.
       * Particularly, (0, 0, 1) is the rotation axis of the nearer joint.
       *
       \**********************************************************************************/

SkeletalMesh::SkeletonModifier modifier;

static void fingerAction(float* angles_array) {
    float _angle;
    _angle = angles_array[0];
    modifier["thumb_proximal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), _angle, glm::fvec3(0.0, 0.0, 1.0));
    _angle = angles_array[1];
    modifier["thumb_intermediate_phalange"] = glm::rotate(glm::identity<glm::mat4>(), _angle, glm::fvec3(0.0, 0.0, 1.0));
    _angle = angles_array[2];
    modifier["thumb_distal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), _angle, glm::fvec3(0.0, 0.0, 1.0));
    _angle = angles_array[3];
    modifier["index_proximal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), _angle, glm::fvec3(0.0, 0.0, 1.0));
    _angle = angles_array[4];
    modifier["index_intermediate_phalange"] = glm::rotate(glm::identity<glm::mat4>(), _angle, glm::fvec3(0.0, 0.0, 1.0));
    _angle = angles_array[5];
    modifier["index_distal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), _angle, glm::fvec3(0.0, 0.0, 1.0));
    _angle = angles_array[6];
    modifier["middle_proximal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), _angle, glm::fvec3(0.0, 0.0, 1.0));
    _angle = angles_array[7];
    modifier["middle_intermediate_phalange"] = glm::rotate(glm::identity<glm::mat4>(), _angle, glm::fvec3(0.0, 0.0, 1.0));
    _angle = angles_array[8];
    modifier["middle_distal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), _angle, glm::fvec3(0.0, 0.0, 1.0));
    _angle = angles_array[9];
    modifier["ring_proximal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), _angle, glm::fvec3(0.0, 0.0, 1.0));
    _angle = angles_array[10];
    modifier["ring_intermediate_phalange"] = glm::rotate(glm::identity<glm::mat4>(), _angle, glm::fvec3(0.0, 0.0, 1.0));
    _angle = angles_array[11];
    modifier["ring_distal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), _angle, glm::fvec3(0.0, 0.0, 1.0));
    _angle = angles_array[12];
    modifier["pinky_proximal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), _angle, glm::fvec3(0.0, 0.0, 1.0));
    _angle = angles_array[13];
    modifier["pinky_intermediate_phalange"] = glm::rotate(glm::identity<glm::mat4>(), _angle, glm::fvec3(0.0, 0.0, 1.0));
    _angle = angles_array[14];
    modifier["pinky_distal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), _angle, glm::fvec3(0.0, 0.0, 1.0));
}

static void restoreState() {
    float _angles_array[15] = { .0f, .0f, .0f, .0f, .0f, .0f, .0f, .0f, .0f, .0f, .0f, .0f, .0f, .0f, .0f };
    fingerAction(_angles_array);
    float metacarpals_angle = 0.0f;
    modifier["metacarpals"] = glm::rotate(glm::identity<glm::mat4>(), metacarpals_angle, glm::fvec3(0.0, 0.0, 1.0));
}

static void orchidFinger() {
    float _angles_array[15] = { M_PI / 12, .0f, M_PI / 6, .0f, .0f, .0f, M_PI / 3.0, M_PI / 4.0, M_PI / 6.0, M_PI / 3.0, M_PI / 4.0, M_PI / 6.0, .0f, .0f, .0f };
    fingerAction(_angles_array);
}

static void fist() {
    float _angles_array[15] = {  M_PI / 6, M_PI / 9, M_PI / 2, M_PI / 2, M_PI / 2, M_PI / 6, M_PI / 2, M_PI / 2, M_PI / 3, M_PI / 2, M_PI / 2, M_PI / 6, M_PI / 2, M_PI / 2, M_PI / 6};
    fingerAction(_angles_array);
    float modify_angle = M_PI / 2;
    modifier["index_proximal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), modify_angle, glm::fvec3(0.0, -0.2, 1.0));
    modifier["middle_proximal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), modify_angle, glm::fvec3(0.0, -0.1, 1.0));
    modifier["middle_proximal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), modify_angle, glm::fvec3(0.0, -0.05, 1.0));
}

static void tao() {
    float _angles_array[15] = { .0f, M_PI / 15.0, 2 * M_PI / 15.0, 3 * M_PI / 15.0, 4 * M_PI / 15.0, 5 * M_PI / 15.0, 6 * M_PI / 15.0, 7 * M_PI / 15.0, 6 * M_PI / 15.0, 5 * M_PI / 15.0, 4 * M_PI / 15.0, 3 * M_PI / 15.0, 2 * M_PI / 15.0, 1 * M_PI / 15.0, .0f };
    fingerAction(_angles_array);
}

static void thumbAnime(float delta_time) {
    delta_time = fmod(delta_time, 4.0);
    if (delta_time > 2.0) 
        delta_time = 4.0 - delta_time;
    float _angle = 75.0 * delta_time / 360.0 * M_PI;
    modifier["thumb_proximal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), _angle, glm::fvec3(0.0, 0.0, 1.0));
}

static void indexAnime(float delta_time) {
    delta_time = fmod(delta_time, 4.0);
    if (delta_time > 2.0)
        delta_time = 4.0 - delta_time;
    float _angle = 75.0 * delta_time / 360.0 * M_PI;
    modifier["index_proximal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), _angle, glm::fvec3(0.0, 0.0, 1.0));
}

static void middleAnime(float delta_time) {
    delta_time = fmod(delta_time, 4.0);
    if (delta_time > 2.0)
        delta_time = 4.0 - delta_time;
    float _angle = 75.0 * delta_time / 360.0 * M_PI;
    modifier["middle_proximal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), _angle, glm::fvec3(0.0, 0.0, 1.0));
}

static void ringAnime(float delta_time) {
    delta_time = fmod(delta_time, 4.0);
    if (delta_time > 2.0)
        delta_time = 4.0 - delta_time;
    float _angle = 75.0 * delta_time / 360.0 * M_PI;
    modifier["ring_proximal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), _angle, glm::fvec3(0.0, 0.0, 1.0));
}

static void pinkyAnime(float delta_time) {
    delta_time = fmod(delta_time, 4.0);
    if (delta_time > 2.0)
        delta_time = 4.0 - delta_time;
    float _angle = 75.0 * delta_time / 360.0 * M_PI;
    modifier["pinky_proximal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), _angle, glm::fvec3(0.0, 0.0, 1.0));
}



static void showNum_0() {
    fist();
}

static void showNum_1() {
    float _angles_array[15] = { M_PI / 8 , M_PI / 18 , M_PI / 3 , - M_PI / 36, - M_PI / 12, 0.0f, M_PI / 3 , M_PI / 4, M_PI / 6, M_PI / 3 , M_PI / 4, M_PI / 6, M_PI / 3 , M_PI / 4, M_PI / 6};
    fingerAction(_angles_array);
}

static void showNum_2() {
    float _angles_array[15] = { M_PI / 6 , M_PI / 18 , M_PI / 3 , -M_PI / 36, -M_PI / 12, 0.0f, -M_PI / 36, -M_PI / 12, 0.0f, M_PI / 3 , M_PI / 4, M_PI / 6, M_PI / 3 , M_PI / 4, M_PI / 6 };
    fingerAction(_angles_array);
}

static void showNum_3() {
    float _angles_array[15] = { M_PI / 8 , M_PI / 18 , M_PI / 3 , 70 * M_PI / 180, 80 * M_PI / 180, M_PI / 6, -M_PI / 36, -M_PI / 12, 0.0f , -M_PI / 36, -M_PI / 12, 0.0f , -M_PI / 36, -M_PI / 12, 0.0f };
    fingerAction(_angles_array);
}

static void fistAnime(float delta_time) {
    delta_time = fmod(delta_time, 2);
    delta_time = (pow(10, delta_time) - 1) / 99;
    float _angles_array[15] = 
    { delta_time * M_PI / 6, delta_time * M_PI / 9, delta_time * M_PI / 2, delta_time * M_PI / 2, delta_time * M_PI / 2, delta_time * M_PI / 6, delta_time * M_PI / 2, delta_time * M_PI / 2, delta_time * M_PI / 3, delta_time * M_PI / 2, delta_time * M_PI / 2, delta_time * M_PI / 6, delta_time * M_PI / 2, delta_time * M_PI / 2, delta_time * M_PI / 6 };
    fingerAction(_angles_array);
    float modify_angle = delta_time * M_PI / 2;
    modifier["index_proximal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), modify_angle, glm::fvec3(0.0, -0.2, 1.0));
    modifier["middle_proximal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), modify_angle, glm::fvec3(0.0, -0.1, 1.0));
    modifier["middle_proximal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), modify_angle, glm::fvec3(0.0, -0.05, 1.0));
}

static void flickAnime(float delta_time) {
    delta_time = fmod(delta_time, 0.4) / 0.4;
    float _angles_array[15] = { 32 * M_PI / 180 - delta_time * M_PI / 18, M_PI / 18 - delta_time * M_PI / 18, M_PI / 6 - delta_time * M_PI / 6, .0f, M_PI / 18 - delta_time * M_PI / 36, .0f - delta_time * M_PI / 18, M_PI / 6, 10 * M_PI / 36 - delta_time * 10 * M_PI / 36, 12 * M_PI / 36 - delta_time * 12 * M_PI / 36, M_PI / 18 - delta_time * M_PI / 18, M_PI / 18 - delta_time * M_PI / 18, M_PI / 36 - delta_time * M_PI / 18, -M_PI / 12, -M_PI / 6 - delta_time * M_PI / 18, -M_PI / 36 };
    fingerAction(_angles_array);
}

static void shakeHandAnime(float delta_time) {
    delta_time = fmod(delta_time, 0.8);
    float metacarpals_angle = sin(delta_time * 2 * M_PI / 0.8) * M_PI / 6;
    modifier["metacarpals"] = glm::rotate(glm::identity<glm::mat4>(), metacarpals_angle, glm::fvec3(0.0, 1.0, 0.0));
}
