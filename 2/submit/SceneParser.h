#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "Scene.h"

using namespace std;

//these codes are dependant on how the input is formated
string PARSECODE_SPHERE = "SPHERE";
string PARSECODE_DIFFUSE = "DIFFUSE";
string PARSECODE_BOX = "BOX";
string PARSECODE_AMBIENT = "AMBIENT";
string PARSECODE_CAMERA = "CAMERA";
string PARSECODE_LIGHT = "LIGHT";
string PARSECODE_METAL = "METAL";

glm::vec3 ExtractVector(string input)
{
	glm::vec3 result;
	//iterate through the input to get the indices of the dividing chars
	//dividing chars: "(", ",", ")"

	//Make a substring from input removing the ( and ) characters
	int inputsize = input.size();
	string sub_input = input.substr(1, (inputsize-2));

	istringstream mstream(sub_input);
	string currentNum;
	int i = 0;
	while (std::getline(mstream, currentNum, ','))
	{
		result[i] = stof(currentNum);
		i++;
	}

	return result;
}
void ParseSceneFromFile(string filepath, SceneStruct& scene)
{
	std::ifstream infile;
	std::string inputLine;
	infile.open(filepath);
	if (!infile)
	{
		std::cerr << "ERRROR: File not Found";
		return;
	}
	std::string input;
	std::vector<std::string> lines;
	while (getline(infile, inputLine))//return file
	{
		lines.push_back(inputLine);
	}
	infile.close();
	//temp variables read from the lines word by word
	CameraObject inputCamera;
	glm::vec3 inputSphereCenter;
	glm::vec3 inputBoxCorner;
	Material inputMaterial;
	float inputFloat;
	float inputRadiusFloat;
	//these will be the vectors filled with the raycasting objects
	vector<SphereObject> inputSpheres;

	glm::vec3 inputAmbient;
	glm::vec3 inputDiffuse;
	//we will use booleans to keep track of what object we are creating
	bool currentlyMakingSphere = false;	//this starts as false
	
	Material currentMaterialBeingMade = Material::E_DIFFUSE;
	for (auto it = lines.begin(); it != lines.end(); ++it)
	{
		//sanity check to skip comments with #
		if ((*it)[0] == '#')
		{
			continue;
		}
		stringstream sstream(*it);
		string firstword;
		string vec3_word;
		string x_str, y_str, z_str;
		getline(sstream, firstword, ' ');
		//debug firstword
		firstword;
		//check the first word to see what it is
		//now that we have the first word, we need to check what it is
		if (firstword == PARSECODE_SPHERE)
		{
			currentlyMakingSphere = true;	//this will be true until we are done making a sphere
			
			//if this is a sphere next up is the vec3 sphere center
			//get the next word
			getline(sstream, vec3_word, ' ');
			//pass the word into our vec3 extractor
			inputSphereCenter = ExtractVector(vec3_word);	//spherecenter
			//now we get the next word to get the scale
			getline(sstream, vec3_word, ' ');
			inputRadiusFloat = stof(vec3_word);
		}
		else if (firstword == PARSECODE_AMBIENT)
		{
			getline(sstream, vec3_word, ' ');
			inputAmbient = ExtractVector(vec3_word);
		}
		else if (firstword == PARSECODE_CAMERA)
		{
			//pos
			getline(sstream, vec3_word, ' ');
			glm::vec3 v = ExtractVector(vec3_word);
			inputCamera.SetCPosition(v);
			//target
			getline(sstream, vec3_word, ' ');
			v = ExtractVector(vec3_word);
			inputCamera.SetTPosition(v);
			//up
			getline(sstream, vec3_word, ' ');
			v = ExtractVector(vec3_word);
			inputCamera.SetUpVector(v);
			//focal length
			getline(sstream, vec3_word, ' ');
			float f = stof(vec3_word);
			inputCamera.SetFocalLength(f);
		}
		else if (firstword == PARSECODE_DIFFUSE)
		{
			//get the next word
			getline(sstream, vec3_word, ' ');
			//pass the word into our vec3 extractor
			inputDiffuse = ExtractVector(vec3_word);
			currentMaterialBeingMade = Material::E_DIFFUSE;
		}
		else if (firstword == "")
		{
			if (currentlyMakingSphere == true)
			{
				if (currentMaterialBeingMade == Material::E_DIFFUSE)
				{
					//create a sphere using the current values inside and add to the vector
					SphereObject sph = SphereObject(inputSphereCenter, inputDiffuse, inputRadiusFloat, Material::E_DIFFUSE);
					inputSpheres.push_back(sph);
					currentlyMakingSphere = false; //reset this to false;
				}
				// default is just diffuse using the current values
				else
				{
					//create a sphere using the current values inside and add to the vector
					SphereObject sph = SphereObject(inputSphereCenter, inputDiffuse, inputRadiusFloat, Material::E_DIFFUSE);
					inputSpheres.push_back(sph);
					currentlyMakingSphere = false; //reset this to false;
				}
			}
		}
		//unknown, continue
		else
		{
			std::cout << "unknown parsecode" << std::endl;
			continue;
		}
	}
	//return all of the elements of the scene in the parameter struct
	SceneStruct inputScene;
	inputScene.mSceneCamera = inputCamera;
	inputScene.mSphereObjs = inputSpheres;
	inputScene.SceneAmbient = inputAmbient;
	scene = inputScene;
	return;
}