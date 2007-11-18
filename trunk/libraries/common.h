#ifndef COMMON_H
#define COMMON_H

/**
 * Iterates over the contents of an STL container.
 * @param iter_type The type of iterator for @p var.
 * @param container The container to iterate over.
 * @param var The variable name for the iterator.
 */
#define FOREACH(iter_type, container, var) \
	for (iter_type var = (container).begin(); var != (container).end(); ++var)

/**
 * Iterates over the children of an XML node.
 * @param parent The parent of the child nodes to iterate over.
 * @param var The variable name for the iterator.
 */
#define FOREACH_XMLCHILD(parent, var) \
	for (xmlNode *var = (parent)->children; var != NULL; var = var->next)

#define FOREACH_XMLATTR(parent, var) \
	for (xmlAttr *var = (parent)->properties; var != NULL; var = var->next)

#endif
