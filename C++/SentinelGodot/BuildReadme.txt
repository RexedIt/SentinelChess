1. Install Godot editor.
2. Install godot_cpp version in different directory, use same version as that of #1
3. Make sure you have scons installed, easy using pip install scons.
3. Export the API JSON from Godot itself, using the console exe
	Example: godot_v4.2.1-stable_win64_console.exe --dump-extension-api
4. Now, we want to run scons to build godot_pp.
	scons platform=windows custom_api_file="[Your Path]extension_api.json"
5. Update your environment and add a GODOT_CPP environment variable, and set it to the path you installed godot_cpp in step 2, with no final \ or /
6. You can now build this project, the godot plugin, using this command:
	scons platform=windows
	and the resulting plugins should work with the demo project and are included in demo/bin folder for your platform.

This URL is helpful in the early steps.
https://docs.godotengine.org/en/stable/tutorials/scripting/gdextension/gdextension_cpp_example.html