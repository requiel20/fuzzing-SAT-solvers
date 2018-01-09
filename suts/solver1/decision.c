#include "decision.h"

#include <stdlib.h>


LitTable
init_node(Literal literal, double score)
{
    LitTable node = malloc(sizeof(PriorityQueue));
    node->literal = literal;
    node->score = score;
    node->left = NULL;
    node->right = NULL;
    node->height = 1;
    return node;
}

void
rotate_right(LitTable *node)
{
    LitTable val = *node;
    LitTable left = val->left;
    LitTable left_right = left->right;
 
    left->right = *node;
    val->left = left_right;
    val->height = 1 + MAX(get_height(val->left), get_height((val->right)));
    left->height = 1 + MAX(get_height(left->left), get_height(left->right));
    *node = left;
}

void
rotate_left(LitTable *node)
{
    LitTable val = *node;
    LitTable right = val->right;
    LitTable right_left = right->left;
 
    right->left = val;
    val->right = right_left;
    val->height = 1 + MAX(get_height(val->left), get_height((val->right)));
    right->height = 1 + MAX(get_height(right->left), get_height(right->right));
    *node = right;
}

int
get_height(LitTable table)
{
    return table ? table->height : 0;
}

int get_balance(LitTable table)
{
    if (!table) {
        return 0;
    }
    int l_height = get_height(table->left);
    int r_height = get_height(table->right);
    return l_height - r_height;
}

LitTable
insert_literal(LitTable table, Literal literal, double score)
{
    if (!table) {
        LitTable node = init_node(literal, score);
        return node;
    }

    if (table->score <= score) {
        table->right = insert_literal(table->right, literal, score);
    } else {
        table->left = insert_literal(table->left, literal, score);
    }
    int l_height = get_height(table->left);
    int r_height = get_height(table->right);
    table->height = 1 + MAX(l_height, r_height);
    int balance = get_balance(table);
    if (balance > 1 && score < table->left->score) {
        rotate_right(&table);
    } else if (balance > 1 && score >= table->left->score) {
        rotate_left(&table->left);
        rotate_right(&table);
    } else if (balance < -1 && score >= table->right->score) {
        rotate_left(&table);
    } else if (balance < -1 && score < table->right->score) {
        rotate_right(&table->right);
        rotate_left(&table);
    }
    return table;
}

LitTable
find_min(LitTable table)
{
    LitTable node = table;
    while (node->left) {
        node = node->left;
    }
    return node;
}

LitTable
remove_node(LitTable table, Literal literal, double score)
{
    if (table->score != score && !equals(literal, table->literal)) {
        return table;
    }
    if (!table->left && !table->right) {
        return NULL;
    }

    if (!equals(literal, table->literal)) {
        return table;
    }

    LitTable node;

    if (table->left && table->right) {
        LitTable node = extract_max(&(table->left));
        node->left = table->left;
        node->right = table->right;
        free(table);
        return node;
    } else if (table->left) {
        node = table->left;
        table->left = NULL;
    } else {
        node = table->right;
        table->right = NULL;
    }
    return node;
}

LitTable
delete_literal(LitTable table, Literal literal, double score)
{
    if (!table) {
        return table;
    }
    if (table->score <= score) {
        if (!equals(table->literal, literal))
            table->right = delete_literal(table->right, literal, score);
        else
            table = remove_node(table, literal, score);
    } else if (table->score > score) {
        table->left = delete_literal(table->left, literal, score);
    } else {
        table = remove_node(table, literal, score);
    }

    if (!table) {
        return table;
    }

    table->height = 1 + MAX(get_height(table->left), get_height(table->right));

    int balance = get_balance(table);

    if (balance > 1 && get_balance(table->left) >= 0) {
        rotate_right(&table);
    } else if (balance > 1 && get_balance(table->left) < 0) {
        rotate_left(&table->left);
        rotate_right(&table);
    } else if (balance < -1 && get_balance(table->right) <= 0) {
        rotate_left(&table);
    } else if (balance < -1 && get_balance(table->right) > 0) {
        rotate_right(&table->right);
        rotate_left(&table);
    }

    return table;
}

LitTable
extract_max(LitTable *table)
{
    LitTable *node = table;
    bool flag = false;
    LitTable path[MAX_TREE_HEIGHT];
    LitTable *parent = table;
    memset(path, 0, MAX_TREE_HEIGHT);
    int i = 0;

    while ((*node)->right) {
        flag = true;
        parent = node;
        path[i] = *parent;
        node = &(*node)->right;
        i++;
    }
    LitTable bump = *node;

    if (!flag && !(*table)->left) {
        *table = NULL;
        return bump;
    } else if (!flag) {
        *table = (*table)->left;
        bump->left = NULL;
        return bump;
    } else if (bump->left) {
        (*parent)->right = bump->left;
        bump->left = NULL;
    } else  {
        (*parent)->right = NULL;
    }
    // Update height of all the nodes we have visited so far.
    for (int j = i - 1; j >= 0; j--) {
        path[j]->height = 1 + MAX(
            get_height(path[j]->left), get_height(path[j]->right));
    }

    int parent_balance = get_balance(*parent);
    int child_balance = get_balance((*parent)->left);

    if (parent_balance > 1 && child_balance >= 0) {
        rotate_right(parent);
    } else if (parent_balance > 1 && parent_balance < 0) {
        rotate_left(&(*parent)->left);
        rotate_right(parent);
    }

    return bump;
}
