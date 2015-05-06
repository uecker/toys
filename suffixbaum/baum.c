/* $Id: baum.c,v 1.2 2001/07/14 21:25:07 martin Exp martin $
 * 
 * $Log: baum.c,v $
 * Revision 1.2  2001/07/14 21:25:07  martin
 * algorithm finished
 *
 * Revision 1.1  2001/07/02 20:45:25  martin
 * Initial revision
 *
 * */


#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "baum.h"



struct tree_s {

	tree child;
	tree next;

	const char* start;	/* mark of edge */
	const char* end;	/* first char behind */

	int from;		/* reachable suffixes */
	int to;
};








static tree new_tree(const char* start, const char* end,
			tree next, tree child, int suffix);
static tree walk(tree root, const char** text, bool split);
static void print_tree(tree t, int depth, int column);
static int renumber(tree t, int nr, int table[]);












/* new_tree
 *
 * Description:
 * 	Creates a new tree node.
 *
 * Parameter:
 * 	const char* start	start of mark
 *	const char* end		end of mark
 *	tree next		neighbor	
 *	tree child		list of childs
 *	int suffix		suffix
 *
 * Result:
 * 	tree			created tree node
 * */

static tree new_tree(const char* start, const char* end,
			tree next, tree child, int suffix)
{	
	tree n = (tree)malloc(sizeof(struct tree_s));

	if (NULL == n) {
		perror(__func__);
		abort();
	}

	n->next = next;
	n->child = child;

	n->start = start;
	n->end = end;

	n->from = suffix;
	n->to = suffix;

	return n;
}







/* delete_tree
 *
 * */

void delete_tree(tree root)
{
	if (NULL != root) {

		delete_tree(root->child);
		delete_tree(root->next);
		free(root);
	}
}






/* walk
 *
 * Description:
 * 	Matches a pattern against the marks on the edges
 * 	of a suffix tree. Optionally splits edges if the
 * 	search doesnt terminate in a tree node.
 *
 * Parameter:
 * 	tree root
 * 	const char** text	pattern
 * 	bool split		split tree?
 *
 * Result:
 * 	tree			last visited node
 * */

static tree walk(tree root, const char** text, bool split)
{
	tree t = root->child;
	const char* e;

	while (NULL != t) {

		/* matching... */

		for (	e = t->start; 

			(e < t->end) 
			&& (*e == **text)
			&& ('\0' != **text); 
			
			(*text)++, e++);

		if (e == t->start) {		/* wrong edge */

			t = t->next;
			continue;
		}
		
		if (split && (e != t->end)) {	/* spit edge */

			t->child = new_tree(	e, t->end, NULL, 
						t->child, t->to);
			t->end = e;
		} 
			
		root = t;
		t = t->child;
	}

	return root;
}







/* renumber
 *
 * Description:
 * 	- Renumbers the leafs of a tree in depth first order.
 * 	- Caclulates reachability intervals for inner nodes.
 * 	- Creates a permutation table where the old numbers of the
 * 	leaf nodes are mapped to the new numbers.
 *
 * Parameter:
 * 	tree t		tree to be renumbered
 * 	int nr		start number
 *	int table[]	permutation table
 *
 * Result:
 * 	int		next unused number	
 *
 * */

static int renumber(tree t, int nr, int table[])
{
	t->from = nr;

	if (NULL == t->child) {		/* leaf */

		table[t->from] = t->to;
		t->to = nr++;

	} else {

		nr = renumber(t->child, nr, table);
		t->to = nr;
	}
	
	if (NULL != t->next)
		nr = renumber(t->next, nr, table);

	return nr;
}








/* create_suffixtree
 *
 * */

struct suffixtree_s create_suffixtree(const char* text)
{
	int laenge = strlen(text);
	struct suffixtree_s rt;
	const char* pos;
	tree t;
	int i;

	rt.text = text;
	rt.root = new_tree(text, text, NULL, NULL, 0);
	rt.table = (int*)malloc((laenge + 1) * sizeof(int));

	if (NULL == rt.table) {
		perror(__func__);
		abort();
	}


	for (i = 0; i <= laenge; i++) {

		pos = text + i;

		t = walk(rt.root, &pos, true);

		t->child = new_tree(pos, text + laenge + 1, 
				    t->child, NULL, i);
	}

	rt.size = renumber(rt.root, 0, rt.table);

	assert(rt.size == laenge + 1);

	return rt;
}








/* find
 *
 * */

void find(tree root, const char* pattern, struct find_result_s* result)
{
	tree t = walk(root, &pattern, false);

	if ('\0' == *pattern) {		/* completed match */

		result->from = t->from;
		result->to = t->to;

	} else {

		result->from = 0;
		result->to = 0;
	}
}








/* print_tree
 *
 * */

static void print_tree(tree t, int depth, int column)
{
	int laenge;

	if (NULL == t) 
		return;

	laenge = t->end - t->start;

	printf("%-*d <%-*.*s> (%d..%d)\n", 2 + column, depth, 
			laenge, laenge, t->start, t->from, t->to);

	print_tree(t->child, depth + 1, column + laenge);
	print_tree(t->next, depth, column);
}






#if TEST_BAUM
int main()
{
	const char* string = "anana";
	const char* pattern = "an";

	struct suffixtree_s st = create_suffixtree(string, strlen(string));
	struct find_result_s r;
	int i;
	
	print_tree(st.root, 0, 0);

	printf("\n\"%s\" in \"%s\":\n\n", pattern, string);

	find(st.root, pattern, &r);

	for (i = r.from; i < r.to; i++)
		printf("\t%2d: %.10s\n", st.table[i], 
				string + st.table[i]);

	delete_tree(st.root);

	exit(0);
}
#endif


