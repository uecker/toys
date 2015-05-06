/* $Id: baum.h,v 1.2 2001/07/14 21:25:15 martin Exp martin $
 *
 * $Log: baum.h,v $
 * Revision 1.2  2001/07/14 21:25:15  martin
 * algorithm finished
 *
 * Revision 1.1  2001/07/02 20:33:24  martin
 * Initial revision
 *
 * */

#ifndef __BAUM_H
#define __BAUM_H	1




struct tree_s;
typedef struct tree_s* tree;


struct suffixtree_s {

	tree root;		/* suffix tree */
	const char* text;	/* text */

	int size;		/* text length + 1 */
	int* table;		/* leaf number to position */
};


struct find_result_s { 
	int from; 
	int to; 
};




/* create_suffixtree
 *
 * Descritpion:
 * 	Calculates the suffix tree for a given string.
 *
 * Parameter:
 * 	const char* text
 *
 * Result:
 * 	struct suffixtree_s
 *
 * */

extern struct suffixtree_s create_suffixtree(const char* text);



/* find
 *
 * Description:
 * 	Looks up a pattern in a given suffix tree and
 * 	returns all matches.
 *
 * Parameter:
 * 	tree root
 * 	const char* pattern
 * 	struct find_result_s* result
 *
 * Result:
 * 	void
 *
 * */

extern void find(tree root, const char* pattern, 
			struct find_result_s* result);



/* delete_tree
 *
 * Description:
 * 	Deletes a tree.
 *
 * Parameter:
 * 	tree root
 *
 * Result:
 * 	void
 *
 * */

extern void delete_tree(tree root);



#endif 

