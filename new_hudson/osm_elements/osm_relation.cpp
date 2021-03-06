#include "osm_relation.h"
using namespace ns_osm;

/*================================================================*/
/*                  Constructors, destructors                     */
/*================================================================*/

Osm_Relation::Osm_Relation(const QString& id)
    : Osm_Object(Osm_Object::Type::RELATION),
      Osm_Info(id)
{
	mn_nodes = 0;
	mn_ways = 0;
	mn_relations = 0;
	//m_size = 0;
	//reg_osm_object(this);
}

Osm_Relation::Osm_Relation() : Osm_Object(Osm_Object::Type::RELATION) {
	mn_nodes = 0;
	mn_ways = 0;
	mn_relations = 0;
	//m_size = 0;
	//reg_osm_object(this);
}

Osm_Relation::~Osm_Relation() {
	//unreg_osm_object(this);
	emit_delete();
}

/*================================================================*/
/*                      Protected methods                         */
/*================================================================*/

void Osm_Relation::handle_event_delete(Osm_Node& node) {
	mn_nodes -= m_nodes_list.removeAll(&node);
	m_roles_hash.remove(reinterpret_cast<Osm_Relation&>(node).get_inner_id());
	emit_update(Meta(NODE_DELETED).set_subject(node));
}

void Osm_Relation::handle_event_delete(Osm_Way& way) {
	mn_ways -= m_ways_list.removeAll(&way);
	m_roles_hash.remove(reinterpret_cast<Osm_Relation&>(way).get_inner_id());
	emit_update(Meta(WAY_DELETED).set_subject(way));
}

void Osm_Relation::handle_event_delete(Osm_Relation& relation) {
	mn_relations -= m_relations_list.removeAll(&relation);
	m_roles_hash.remove(reinterpret_cast<Osm_Relation&>(relation).get_inner_id());
	emit_update(Meta(RELATION_DELETED).set_subject(relation));
}

void Osm_Relation::handle_event_update(Osm_Node& node) {
	emit_update(Meta(NODE_UPDATED).set_subject(node));
}

void Osm_Relation::handle_event_update(Osm_Way& way) {
	emit_update(Meta(WAY_UPDATED).set_subject(way));
}

void Osm_Relation::handle_event_update(Osm_Relation& relation) {
	emit_update(Meta(RELATION_UPDATED).set_subject(relation));
}

/*================================================================*/
/*                        Public methods                          */
/*================================================================*/

void Osm_Relation::add(Osm_Node* ptr_node, const QString &role) {
	if (ptr_node == nullptr) {
		set_valid(false);
		return;
	} else if (has(ptr_node)) {
		return;
	}

	subscribe(*ptr_node);
	if (!(ptr_node->is_valid())) {
		set_valid(false);
	}
	m_nodes_list.push_back(ptr_node);
	mn_nodes++;
	set_role(ptr_node, role);
	emit_update(Meta(NODE_ADDED).set_subject(*ptr_node));
}



void Osm_Relation::add(Osm_Way* ptr_way, const QString &role) {
	if (ptr_way == nullptr) {
		set_valid(false);
		return;
	} else if (has(ptr_way)) {
		return;
	}

	subscribe(*ptr_way);
	if (!(ptr_way->is_valid())) {
		set_valid(false);
	}
	m_ways_list.push_back(ptr_way);
	mn_ways++;
	set_role(ptr_way, role);
	emit_update(Meta(WAY_ADDED).set_subject(*ptr_way));
}

void Osm_Relation::add(Osm_Relation* ptr_rel, const QString& role) {
	if (ptr_rel == nullptr || ptr_rel == this) {
		set_valid(false);
		return;
	} else if (has(ptr_rel)) {
		return;
	}

	subscribe(*ptr_rel);
	if (!(ptr_rel->is_valid())) {
		set_valid(false);
	}
	m_relations_list.push_back(ptr_rel);
	mn_relations++;
	set_role(ptr_rel, role);
	emit_update(Meta(RELATION_ADDED).set_subject(*ptr_rel));
}

void Osm_Relation::remove(Osm_Node* ptr_node) {
	if (ptr_node == nullptr) {
		return;
	}
	mn_nodes -= m_nodes_list.removeAll(ptr_node);
	m_roles_hash.remove(reinterpret_cast<Osm_Relation*>(ptr_node)->get_inner_id());
	unsubscribe(*ptr_node);
	emit_update(Meta(NODE_DELETED).set_subject(*ptr_node));
}

void Osm_Relation::remove(Osm_Way* ptr_way) {
	if (ptr_way == nullptr) {
		return;
	}
	mn_ways -= m_ways_list.removeAll(ptr_way);
	m_roles_hash.remove(reinterpret_cast<Osm_Relation*>(ptr_way)->get_inner_id());
	unsubscribe(*ptr_way);
	emit_update(Meta(WAY_DELETED).set_subject(*ptr_way));
}

void Osm_Relation::remove(Osm_Relation* ptr_rel) {
	if (ptr_rel == nullptr) {
		return;
	}

	mn_relations -= m_relations_list.removeAll(ptr_rel);
	m_roles_hash.remove(reinterpret_cast<Osm_Relation*>(ptr_rel)->get_inner_id());
	unsubscribe(*ptr_rel);
	emit_update(Meta(RELATION_DELETED).set_subject(*ptr_rel));
}

bool Osm_Relation::has(Osm_Node* p_node) const {
	if (p_node == nullptr) {
		return false;
	}
	return m_nodes_list.contains(p_node);
}

bool Osm_Relation::has(Osm_Way* p_way) const {
	if (p_way == nullptr) {
		return false;
	}
	return m_ways_list.contains(p_way);
}

bool Osm_Relation::has(Osm_Relation* p_rel) const {
	if (p_rel == nullptr) {
		return false;
	}
	return m_relations_list.contains(p_rel);
}

void Osm_Relation::set_role(Osm_Object* ptr_object, const QString& role) {
	if (ptr_object == nullptr) {
		return;
	}
	m_roles_hash[static_cast<Osm_Relation*>(ptr_object)->get_inner_id()] = role;
	emit_update(Meta(RELATION_MEMBER_ROLE_SET).set_subject(*ptr_object));
}

unsigned Osm_Relation::get_size() const {
	return count_nodes() + count_ways() + count_relations();
}

const QString Osm_Relation::get_role(Osm_Object* ptr_object) const {
	return m_roles_hash[static_cast<Osm_Relation*>(ptr_object)->get_inner_id()];
}

const QList<Osm_Node*>& Osm_Relation::get_nodes() const {
	return m_nodes_list;
}

const QList<Osm_Way*>& Osm_Relation::get_ways() const {
	return m_ways_list;
}

const QList<Osm_Relation*>& Osm_Relation::get_relations() const {
	return m_relations_list;
}

unsigned short Osm_Relation::count_nodes() const {
	return mn_nodes;
}

unsigned short Osm_Relation::count_ways() const {
	return mn_ways;
}

unsigned short Osm_Relation::count_relations() const {
	return mn_relations;
}
