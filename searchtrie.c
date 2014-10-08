#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define KEY_SIZE 64
int memusage;

////////////////////////////////////////////////////////////////////////////////

typedef struct BinaryTree {
	char key;
	void * value;
	struct BinaryTree * left;
	struct BinaryTree * right;
} BinaryTree;

BinaryTree * createNode(char key, void * value) {
	BinaryTree * node = malloc(sizeof(BinaryTree));
	memusage += sizeof(BinaryTree);
	node->key = key;
	node->value = value;
	node->left = NULL;
	node->right = NULL;
	return node;
}

////////////////////////////////////////////////////////////////////////////////

typedef BinaryTree Trie;

Trie ** buildPath(Trie ** node, char key) {
	while (*node != NULL) {
		if (key < (*node)->key) {
			node = &(*node)->left;
		} else if (key > (*node)->key) {
			node = &(*node)->right;
		} else {
			return (Trie **)&(*node)->value;
		}
	}

	*node = createNode(key, NULL);
	return (Trie **)&(*node)->value;
}

void updateTrie(Trie ** node, const char * key, void * value) {
	while (*key != '\n') {
		node = buildPath(node, *key++);
	}

	*node = createNode('\0', value);
}

Trie * tracePath(Trie * node, char key) {
	while (node != NULL) {
		if (key < node->key) {
			node = node->left;
		} else if (key > node->key) {
			node = node->right;
		} else {
			return node->value;
		}
	}

	return NULL;
}

void traverseTrie(Trie * node, char * buffer, int i) {
	if (node != NULL) {
		traverseTrie(node->left, buffer, i);

		buffer[i] = node->key;
		if (node->key == '\0') {
			printf("%s: %i\n", buffer, node->value);
		} else {
			traverseTrie(node->value, buffer, i + 1);
		}

		traverseTrie(node->right, buffer, i);
	}
}

void searchTrie(Trie * node, const char * key) {
	int i = 0;
	char buffer[KEY_SIZE];

	while (*key != '\n') {
		if (node == NULL) return;
		node = tracePath(node, *key);
		buffer[i++] = *key++;
	}

	traverseTrie(node, buffer, i);
}

////////////////////////////////////////////////////////////////////////////////

void loadDictionary(Trie ** trie, const char * filepath) {
	size_t i = 0;
	char buffer[KEY_SIZE];

	FILE * fp = fopen(filepath, "r");
	if (!fp) {
		fprintf(stderr, "Failed to open dictionary: \'%s\'\n", filepath);
		return;
	}

	while (fgets(buffer, sizeof(buffer), fp) != NULL) {
		updateTrie(trie, buffer, (void *)i++);
	}

	fclose(fp);
}

void promptSearch(Trie * trie) {
	char buffer[KEY_SIZE];

	printf("> ");
	while (fgets(buffer, sizeof(buffer), stdin) != NULL) {
		printf("========== Search Results ==========\n");
		searchTrie(trie, buffer);
		printf("====================================\n");
		printf("> ");
	}
}

int main(int argc, const char * argv[]) {
	if (argc < 2) {
		fprintf(stderr, "Usage: %s <dictionary> ... \n", argv[0]);
		return 1;
	}

	Trie * trie = NULL;

	// Load multiple dictionaries from command line into trie
	int i;
	for (i = 1; i < argc; i++) {
		loadDictionary(&trie, argv[i]);
	}

	printf("%s: %i MB\n", "Trie Memory usage", memusage);
	printf("Enter search term:\n");
	promptSearch(trie);

	return 0;
}
