/* Author: Michael Arnold
   Student ID: 160155057
   Last Edited:  26 / 11/ 2019
   */
#include "../../nclgl/window.h"
#include "Renderer.h"
#pragma comment(lib, "nclgl.lib")

int main() {
	Window w("Index Buffers!", 800, 600, false);
	if (!w.HasInitialised()) {
		return -1;
	}
	Renderer renderer(w);


	if (!renderer.HasInitialised()) {
		return -1;
	}
	bool rotationMode = false;
	bool lightRotation = false;
	bool growthInvert = false;
	bool stopGrowth = false;

	renderer.SetCameraMode(rotationMode);
	renderer.SetLightRotation(lightRotation);
	renderer.SetGrowthInvert(growthInvert);
	renderer.SetStopGrowth(stopGrowth);

	w.LockMouseToWindow(true);
	w.ShowOSPointer(false);

	while (w.UpdateWindow() && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE)) {
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		renderer.UpdateScene(w.GetTimer()->GetTimedMS());
		renderer.RenderScene();
		

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_C)) {
			renderer.MoveLightToCameraPos();
		}
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_R)) {
			if (rotationMode)
				rotationMode = false;
			else
				rotationMode = true;
			renderer.SetCameraMode(rotationMode);
		}
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_L)) {
			if (rotationMode)
				lightRotation = false;
			else
				lightRotation = true;
			renderer.SetLightRotation(lightRotation);
		}

		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_I)) {
			if (growthInvert)
				growthInvert = false;
			else
				growthInvert = true;

			renderer.SetGrowthInvert(growthInvert);
		}
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_O)) {
			if (stopGrowth)
				stopGrowth = false;
			else
				stopGrowth = true;

			renderer.SetStopGrowth(stopGrowth);
		}

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_U)) {
			renderer.ChangeLightHeight(true);
		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_J)) {
			renderer.ChangeLightHeight(false);
		}
	}
	return 0;
}