#include <Levels/AnimationTest.h>
#include <System/Objects.h>
#include <Loaders/ObjLoader.h>

/**
 * AnimationTest
 * -------------
 */
void AnimationTest::load() {
	ObjLoader step1("./DemoAssets/animation/step1.obj");
	ObjLoader step2("./DemoAssets/animation/step2.obj");
	ObjLoader step3("./DemoAssets/animation/step3.obj");

	Model* person = new Model(step1);

	person->addMorphTarget(new Model(step1));
	person->addMorphTarget(new Model(step2));
	person->addMorphTarget(new Model(step3));

	person->position = { 0, 0, 1500 };
	person->setColor({ 20, 255, 50 });
	person->scale(70);
	person->startMorph(1000, true);

	add("person", person);

	settings.ambientLightColor = { 0, 50, 255 };
	settings.ambientLightVector = { 0, -1, 1 };
	settings.brightness = 0.3f;
	settings.hasStaticAmbientLight = true;
}

void AnimationTest::onUpdate(int dt, int runningTime) {}
