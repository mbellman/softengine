#include <Scenes/AnimationTest.h>
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

	for (int i = 0; i < 10; i++) {
		Model* person = new Model(step1);

		person->addMorphTarget(new Model(step1));
		person->addMorphTarget(new Model(step2));
		person->addMorphTarget(new Model(step3));

		person->position = { -3250.0f + 750.0f * i, 0, 1500.0f };
		person->transformOrigin = { 200, 0, 0 };
		person->setColor({ 20, 255, 50 });
		person->scale(70);
		person->startMorph(1000, true);

		person->onUpdate = [=](int dt, int runningTime) {
			person->rotateDeg({ 0, 1.0f, 0 });
		};

		add(person);
	}

	settings.ambientLightColor = { 0, 50, 255 };
	settings.ambientLightVector = { 0, -1, 1 };
	settings.brightness = 0.3f;
	settings.hasStaticAmbientLight = true;
}

void AnimationTest::onUpdate(int dt, int runningTime) {
	// Object* person = getObject("person");

	// person->rotateDeg({ 0, 1.0f, 0 });

	// if (runningTime > 5000 && person->isMorphing()) {
	// 	person->stopMorph();
	// }
}
