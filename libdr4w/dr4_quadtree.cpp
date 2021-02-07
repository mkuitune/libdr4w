#include <dr4/dr4_quadtree.h>

void dr4::FieldQuadtreeBuilder::addExisting(std::function<float(float, float)> field) {

	std::stack<size_t> notProcessed;
	for (size_t n = 0; n < tree.nodes.size(); n++)
		notProcessed.push(n);

	while (!notProcessed.empty()) {
		size_t ni = notProcessed.top();
		notProcessed.pop();

		FieldQuadtreeNode& node = tree.nodes[ni];
		
		// Store old node so we can sample the field
		FieldQuadtreeNode oldNode = node;

		node.applyFieldToExisting(field);

		// subdivided only current leafs. Othewise just apply field 
		if (node.childs != 0){
			continue;
		}

		// Find of precision suffices or do we need to subdivide
		auto samplepoints = node.samplepoints();
		float realValues[5];
		for (int i = 0; i < 5; i++) {
			auto pnt = samplepoints.coords[i];
			float oldFieldValue = oldNode.sampleCorners(pnt.x, pnt.y);
			float newFieldValue = field(pnt.x, pnt.y);
			realValues[i] = std::min(oldFieldValue, newFieldValue);
		}
		float diff = samplepoints.sampleDifference(realValues);

		if (diff > threshold) {
			// subdivided
			float d2 = node.d / 2;
			FieldQuadtreeNode n0 = FieldQuadtreeNode::Init(node.x0, node.y0, d2);
			FieldQuadtreeNode n1 = FieldQuadtreeNode::Init(node.x0 + d2, node.y0, d2);
			FieldQuadtreeNode n2 = FieldQuadtreeNode::Init(node.x0 + d2, node.y0 + d2, d2);
			FieldQuadtreeNode n3 = FieldQuadtreeNode::Init(node.x0, node.y0 + d2, d2);

			// Apply sampling of current field and old field. 
			n0.applyField(field);
			n0.applyPrevious(oldNode);
			n1.applyField(field);
			n1.applyPrevious(oldNode);
			n2.applyField(field);
			n2.applyPrevious(oldNode);
			n3.applyField(field);
			n3.applyPrevious(oldNode);

			size_t newIdx = tree.nodes.size();
			node.childs = newIdx;
			tree.nodes.push_back(n0);
			tree.nodes.push_back(n1);
			tree.nodes.push_back(n2);
			tree.nodes.push_back(n3);
			notProcessed.push(newIdx);
			notProcessed.push(newIdx + 1);
			notProcessed.push(newIdx + 2);
			notProcessed.push(newIdx + 3);
		}
	}

}

void dr4::FieldQuadtreeBuilder::addNew(std::function<float(float, float)> field) {
	{
		FieldQuadtreeNode node = FieldQuadtreeNode::Init(x, y, d);
		node.applyField(field);
		tree.nodes.push_back(node);
	}

	std::stack<size_t> notProcessed;
	notProcessed.push(0);

	while (!notProcessed.empty()) {
		size_t ni = notProcessed.top();
		notProcessed.pop();

		FieldQuadtreeNode& node = tree.nodes[ni];
		float diff = node.fieldDifferencesAtSamplepoints(field);

		if (diff > threshold) {
			// subdivided
			float d2 = node.d / 2;
			FieldQuadtreeNode n0 = FieldQuadtreeNode::Init(node.x0, node.y0, d2);
			FieldQuadtreeNode n1 = FieldQuadtreeNode::Init(node.x0 + d2, node.y0, d2);
			FieldQuadtreeNode n2 = FieldQuadtreeNode::Init(node.x0 + d2, node.y0 + d2, d2);
			FieldQuadtreeNode n3 = FieldQuadtreeNode::Init(node.x0, node.y0 + d2, d2);

			n0.applyField(field);
			n1.applyField(field);
			n2.applyField(field);
			n3.applyField(field);

			size_t newIdx = tree.nodes.size();
			node.childs = newIdx;
			tree.nodes.push_back(n0);
			tree.nodes.push_back(n1);
			tree.nodes.push_back(n2);
			tree.nodes.push_back(n3);
			notProcessed.push(newIdx);
			notProcessed.push(newIdx + 1);
			notProcessed.push(newIdx + 2);
			notProcessed.push(newIdx + 3);
		}
	}
}
