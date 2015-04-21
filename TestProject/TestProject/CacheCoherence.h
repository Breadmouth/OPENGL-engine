#include "Application.h"
#include <windows.h>
#include <vector>

class BoundingSphere
{
public:
	BoundingSphere() : centre(0), radius(1) {}
	~BoundingSphere() {}
	void fit(const BoundingSphere& sphere) {
		float d = glm::distance(centre, sphere.centre) +
			sphere.radius;
		if (d > radius)
			radius = d;
	}
	glm::vec4 centre;
	float radius;
};


class Node 
{
public:
	Node() : parent(nullptr), localTransform(1) {}
	virtual ~Node() { for (auto child : children) delete child; }
	void addChild(Node* child) {
		children.push_back(child);
	}
	virtual void update() {
		updateWorldTransform();
		for (auto child : children)
			child->update();
		updateWorldBounds();
	}
	void updateWorldTransform() {
		if (parent == nullptr)
			worldTransform = localTransform;
		else
			worldTransform = parent->worldTransform *
			localTransform;
	}
	void updateWorldBounds() {
		worldBounds.centre = worldTransform * localBounds.centre;
		worldBounds.radius = localBounds.radius;
		for (auto child : children)
			worldBounds.fit(child->worldBounds);
	}
protected:
	Node* parent;
	BoundingSphere localBounds;
	BoundingSphere worldBounds;
	glm::mat4 localTransform;
	glm::mat4 worldTransform;
	std::vector<Node*> children;
};


class CacheCoherence : public Application
{
public:
	CacheCoherence(){}
	virtual void Create()
	{
		root = new Node();
		addChildren(root, 5, 6);

		LARGE_INTEGER perfFreq;
		QueryPerformanceFrequency(&perfFreq);

		LARGE_INTEGER startTime, endTime;

		QueryPerformanceCounter(&startTime);

		//update code here
		root->update();

		QueryPerformanceCounter(&endTime);

		double duration = (endTime.QuadPart - startTime.QuadPart) /
			(double)perfFreq.QuadPart;

		delete root;
	}
	virtual void Destroy(){}
	virtual void Update(float dt){}
	virtual void Draw(){}

	void addChildren(Node* root, int childrenPerNode, int depth) 
	{
		if (depth > 0) {
			for (int i = 0; i < childrenPerNode; i++) {
				Node* child = new Node();
				addChildren(child, childrenPerNode, depth - 1);
				root->addChild(child);
			}
		}
	}

protected:
	Node* root;

};