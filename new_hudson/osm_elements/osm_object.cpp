#include "osm_object.h"
#include "osm_node.h"
#include "osm_way.h"
#include "osm_relation.h"

using namespace ns_osm;

/*================================================================*/
/*                        Static members                          */
/*================================================================*/

long long				Osm_Object::s_osm_id_bound(-1);
long long				Osm_Object::s_inner_id_bound(0xFFFFFFFFFFFFFFFF);
QHash<long long, bool>	Osm_Object::s_id_to_lifestage;

/*================================================================*/
/*                  Constructors, destructors                     */
/*================================================================*/

Osm_Object::Osm_Object() :
    OSM_ID(s_osm_id_bound--),
	INNER_ID(s_inner_id_bound++),
	TYPE(Osm_Object::Type::NODE)
{
	f_is_valid = true;
	m_attrmap[QString("id")] = QString::number(OSM_ID);
	mn_subscribers = 0;
	mn_osm_object_subscribers = 0;
	s_id_to_lifestage[INNER_ID] = true;
	//reg_osm_object(this);
}

Osm_Object::Osm_Object(const Osm_Object::Type type) :
	OSM_ID(s_osm_id_bound--),
	INNER_ID(s_inner_id_bound++),
	TYPE(type)
{
	f_is_valid = true;
	m_attrmap[QString("id")] = QString::number(OSM_ID);
	mn_subscribers = 0;
	mn_osm_object_subscribers = 0;
	s_id_to_lifestage[INNER_ID] = true;
	//reg_osm_object(this);
}


Osm_Object::Osm_Object(const QString &id, const Osm_Object::Type type):
    OSM_ID(id.toLongLong()),
	INNER_ID(s_inner_id_bound++),
	TYPE(type)
{
	f_is_valid = true;
	m_attrmap[QString("id")] = QString::number(OSM_ID);
	if (s_osm_id_bound >= OSM_ID) {
		s_osm_id_bound = (OSM_ID - 1);
	}
	mn_subscribers = 0;
	mn_osm_object_subscribers = 0;
	s_id_to_lifestage[INNER_ID] = true;
}

Osm_Object::~Osm_Object() {
	s_id_to_lifestage[INNER_ID] = false;
	while (!m_subscribers.empty()) {
		m_subscribers.front()->unsubscribe(*this);
	}
}

/*================================================================*/
/*                       Private methods                          */
/*================================================================*/

bool Osm_Object::is_locked(long long id) {
	return !s_id_to_lifestage[id];
}

bool Osm_Object::is_osm_object(Osm_Subscriber* p_subscriber) const {
	if (dynamic_cast<Osm_Node*>(p_subscriber) != nullptr ||
	        dynamic_cast<Osm_Way*>(p_subscriber) != nullptr ||
	        dynamic_cast<Osm_Relation*>(p_subscriber) != nullptr) {
		return true;
	}
	return false;
}

/*================================================================*/
/*                      Protected methods                         */
/*================================================================*/

const Osm_Object::Type Osm_Object::get_type() const {
	return TYPE;
}

void Osm_Object::set_valid(bool f_valid) {
	f_is_valid = f_valid;
}


long long Osm_Object::get_inner_id() const {
	return INNER_ID;
}

void Osm_Object::emit_delete(Osm_Subscriber::Meta meta) {
	const long long THIS_ID = INNER_ID;
	m_active_stack = m_subscribers;
	Osm_Subscriber* p_current_subscriber;
	while (!m_active_stack.empty()) {
		p_current_subscriber = m_active_stack.front();
		p_current_subscriber->set_meta(meta);
		switch (get_type()) {
		case Type::NODE:
			p_current_subscriber->handle_event_delete(*static_cast<Osm_Node*>(this));
			break;
		case Type::WAY:
			p_current_subscriber->handle_event_delete(*static_cast<Osm_Way*>(this));
			break;
		case Type::RELATION:
			p_current_subscriber->handle_event_delete(*static_cast<Osm_Relation*>(this));
			break;
		}
		if (is_locked(THIS_ID)) {
			return;
		}
		if (m_active_stack.empty()) {
			return;
		}
		if (p_current_subscriber == m_active_stack.front()) {
			m_active_stack.pop_front();
			p_current_subscriber->unsubscribe(*this);
		}
	}
}

void Osm_Object::emit_update(Osm_Subscriber::Meta meta) {
	const long long THIS_ID = INNER_ID;
	m_active_stack = m_subscribers;
	Osm_Subscriber* p_current_subscriber;
	while (!m_active_stack.empty()) {
		p_current_subscriber = m_active_stack.front();
		p_current_subscriber->set_meta(meta);
		switch (get_type()) {
		case Type::NODE:
			p_current_subscriber->handle_event_update(*static_cast<Osm_Node*>(this));
			break;
		case Type::WAY:
			p_current_subscriber->handle_event_update(*static_cast<Osm_Way*>(this));
			break;
		case Type::RELATION:
			p_current_subscriber->handle_event_update(*static_cast<Osm_Relation*>(this));
			break;
		}
		if (is_locked(THIS_ID)) {
			return;
		}
		if (m_active_stack.empty()) {
			return;
		}
		if (p_current_subscriber == m_active_stack.front()) {
			m_active_stack.pop_front();
		}
	}
}

/*================================================================*/
/*                        Public methods                          */
/*================================================================*/

void Osm_Object::add_subscriber(Osm_Subscriber& subscriber) {
	//if (m_subscribers.indexOf(&subscriber) == -1) {
	if (!m_subscribers.contains(&subscriber)) {
		m_subscribers.push_back(&subscriber);
		mn_subscribers++;
		if (is_osm_object(&subscriber)) {
			mn_osm_object_subscribers++;
		}
	}
}

void Osm_Object::remove_subscriber(Osm_Subscriber& subscriber) {
	//m_subscribers.remove(&subscriber);
	if (m_subscribers.removeOne(&subscriber)) {
		mn_subscribers--;
		if (is_osm_object(&subscriber)) {
			mn_osm_object_subscribers--;
		}
	}
	m_active_stack.removeOne(&subscriber);
}

int Osm_Object::count_subscribers() const {
	return mn_subscribers;
}

int Osm_Object::count_osm_subscribers() const {
	return mn_osm_object_subscribers;
}

QString Osm_Object::get_attr_value(const QString& key) const {
	return m_attrmap[key];
}

QString Osm_Object::get_tag_value(const QString &key) const {
	return m_tagmap[key];
}

const QMap<QString, QString>& Osm_Object::get_tag_map() const {
	return m_tagmap;
}

const QMap<QString, QString>& Osm_Object::get_attr_map() const {
	return m_attrmap;
}

long long Osm_Object::get_id() const {
	return OSM_ID;
}

void Osm_Object::set_tag(const QString &key, const QString &value) {
	m_tagmap[key] = value;
}

void Osm_Object::set_attr(const QString &key, const QString &value) {
	if (key == "id")
		return;
	m_attrmap[key] = value;
}

void Osm_Object::remove_tag(const QString &key) {
	m_tagmap.remove(key);
}


void Osm_Object::clear_tags() {
	m_tagmap.clear();
}

bool Osm_Object::is_valid() const {
	return f_is_valid;
}
