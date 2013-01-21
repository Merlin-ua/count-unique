#include <iostream>
#include <string>
#include <vector>
#include <map>

using namespace std;

struct rope {
	int height;  // Leafs have 0.
	int value;  // All values below <= this.
	int weight;  // Total count of values below including this one.
	vector<rope*> child;  // Strictly 2 or 3 children.
};

// Assumes values is sorted, because we use it as an index.
rope* construct(const vector<int>& values) {
	vector<rope*> level(values.size());
	for (int i = 0; i < values.size(); ++i) {
		rope* current = new rope;
		current->height = 0;
		current->value = values[i];
		current->weight = 1;
		level[i] = current;
	}
	int height = 1;
	while (level.size() > 1) {
		vector<rope*> new_level;
		int step = 3;
		for (int i = 0; i < level.size(); i += step) {
			if (i + 4 == level.size() || i + 3 > level.size()) {
				step = 2;
			}
			rope* current = new rope;
			current->height = height;
			current->weight = 0;
			for (int j = i; j < i + step; ++j) {
				current->child.push_back(level[j]);
				current->weight += level[j]->weight;
			}
			current->value = level[i + step - 1]->value;
			new_level.push_back(current);
		}
		++height;
		swap(level, new_level);
	}
	return level[0];
}

rope* construct(int value) {
	rope* result = new rope;
	result->height = 0;
	result->value = value;
	result->weight = 1;
	return result;
}

rope* construct(int height, rope* a, rope* b) {
	rope* result = new rope;
	result->height = height;
	result->child.push_back(a);
	result->child.push_back(b);
	result->weight = a->weight + b->weight;
	result->value = b->value;
	return result;
}

rope* construct(int height, rope* a, rope* b, rope* c) {
	rope* result = new rope;
	result->height = height;
	result->child.push_back(a);
	result->child.push_back(b);
	result->child.push_back(c);
	result->weight = a->weight + b->weight + c->weight;
	result->value = c->value;
	return result;
}

rope* merge(rope* left, rope* right) {
	if (left == NULL) {
		return right;
	}
	if (right == NULL) {
		return left;
	}
	if (left->height == right->height) {
		return construct(left->height + 1, left, right);
	} else if (left->height + 1 == right->height) {
		if (right->child.size() == 2) {
			return construct(right->height, left, right->child[0], right->child[1]);
		} else {
			return construct(right->height + 1,
					             construct(right->height, left, right->child[0]),
											 construct(right->height, right->child[1], right->child[2]));
		}
	} else if (left->height == right->height + 1) {
		if (left->child.size() == 2) {
			return construct(left->height, left->child[0], left->child[1], right);
		} else {
			return construct(left->height + 1,
											 construct(left->height, left->child[0], left->child[1]),
					             construct(left->height, left->child[2], right));
		}
	} else if (left->height + 1 < right->height) {
		if (right->child.size() == 2) {
			return merge(merge(left, right->child[0]), right->child[1]);
		} else {
			return merge(merge(left, right->child[0]), merge(right->child[1], right->child[2]));
		}
	} else if (left->height > right->height + 1) {
		if (left->child.size() == 2) {
			return merge(left->child[0], merge(left->child[1], right));
		} else {
			return merge(merge(left->child[0], left->child[1]), merge(left->child[2], right));
		}
	} else {
		printf("fatal error\n");
	}
}


int count_le(rope* rope, int pivot) {
	if (rope->child.empty()) {
		return rope->value <= pivot ? 1 : 0;
	}
	int child = 0;
	int count = 0;
	while (rope->child[child]->value < pivot) {
		count += rope->child[child]->weight;
		++child;
		if (child >= rope->child.size()) {
			return count;
		}
	}
	return count + count_le(rope->child[child], pivot);
}


pair<rope*, rope*> split(rope* value, int pivot) {
	if (value->height == 0) {
		if (value->value > pivot) {
			return make_pair(static_cast<rope*>(NULL), value);
		} else if (value->value < pivot) {
			return make_pair(value, static_cast<rope*>(NULL));
		}
		return pair<rope*, rope*>(NULL, NULL);
	}

	pair<rope*, rope*> subdivide;
	if (value->child.size() == 2) {
		if (value->child[0]->value >= pivot) {
			subdivide = split(value->child[0], pivot);
			return make_pair(subdivide.first, merge(subdivide.second, value->child[1]));
		} else if (value->child[1]->value >= pivot) {
			subdivide = split(value->child[1], pivot);
			return make_pair(merge(value->child[0], subdivide.first), subdivide.second);
		} else {
			return make_pair(value, static_cast<rope*>(NULL));
		}
	} else if (value->child.size() == 3) {
		if (value->child[0]->value >= pivot) {
			subdivide = split(value->child[0], pivot);
			return make_pair(subdivide.first, merge(merge(subdivide.second, value->child[1]), value->child[2]));
		} else if (value->child[1]->value >= pivot) {
			subdivide = split(value->child[1], pivot);
			return make_pair(merge(value->child[0], subdivide.first), merge(subdivide.second, value->child[2]));
		} else if (value->child[2]->value >= pivot) {
			subdivide = split(value->child[2], pivot);
			return make_pair(merge(value->child[0], merge(value->child[1], subdivide.first)), subdivide.second);
		} else {
			return make_pair(value, static_cast<rope*>(NULL));
		}
	} else {
		printf("fatal error\n");
	}
	return pair<rope*, rope*>(NULL, NULL);
}


void print(rope* value, int offset) {
	if (value == NULL) {
		printf("%*s\n", offset, "NULL");
		return;
	}
	if (value->child.empty()) {
		printf("%*d\n", offset, value->value);
	} else {
		printf("%*s,%d,%d\n", offset, "*", value->value, value->weight);
		for (int i = 0; i < value->child.size(); ++i) {
			print(value->child[i], offset + 10);
		}
	}
}

int main() {
	/*
	vector<int> values;

	for (int i = 0; i < 13; ++i) {
		values.push_back(i);
	}
	rope* left = construct(values);

	values.clear();
	for (int i = 13; i < 17; ++i) {
		values.push_back(i);
	}
	rope* right = construct(values);

	rope* test = merge(left, right);
	printf("==========\n");
	print(test, 0);
	printf("==========\n");

	pair<rope*, rope*> divide = split(test, 16);
	printf("----------\n");
	print(divide.first, 0);
	printf("==========\n");
	print(divide.second, 0);
	printf("----------\n");
	*/

	int N, T;
	vector<int> Ns;
	cin >> N;
	for (int i = 0; i < N; ++i) {
		int n;
		cin >> n;
		Ns.push_back(n);
	}

	/*
	for (int i = 0; i < N; ++i) {
		cout << Ns[i] << " ";
	}
	cout << endl;
	*/

	vector<int> next(N, -1);
	vector<int> initial;
	map<int, int> pos;
	for (int i = 0; i < N; ++i) {
		pair<map<int, int>::iterator, bool> ins = pos.insert(make_pair(Ns[i], i));
		if (ins.second) {
			initial.push_back(i);
		} else {
			// The key was already there.
			next[ins.first->second] = i;
			ins.first->second = i;
		}
	}

	/*
	for (int i = 0; i < N; ++i) {
		cout << next[i] << " ";
	}
	cout << endl;
	*/

	vector<rope*> uniques(N);
	uniques[0] = construct(initial);

	cout << "Starting rope..." << endl;
	for (int i = 1; i < N; ++i) {
		pair<rope*, rope*> splitted = split(uniques[i-1], i-1);
		if (splitted.first != NULL) {
			printf("assertion failed for %d\n", i);
		}
		if (next[i-1] != -1) {
			splitted = split(splitted.second, next[i-1]);
			uniques[i] = merge(merge(splitted.first, construct(next[i-1])), splitted.second);
		} else {
			uniques[i] = splitted.second;
		}
		if (i % 100000 == 0) {
			cout << i << " done." << endl;
		}
	}

	/*
	cout << endl;
	for (int i = 0; i < N; ++i) {
		print(uniques[i], 0);
		cout << "-----------" << endl;
	}
	cout << endl;
	*/

	cin >> T;
	for (int i = 0; i < T; ++i) {
		int a, b;
		cin >> a >> b;
		cout << "(" << a << ", " << b << ") = " << count_le(uniques[a], b) << endl;
	}

	return 0;
}
