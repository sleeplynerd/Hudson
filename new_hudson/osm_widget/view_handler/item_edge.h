#ifndef ITEM_EDGE_H
#define ITEM_EDGE_H

#ifndef QT_WIDGETS_H
#define QT_WIDGETS_H
#include <QtWidgets>
#endif /* Include guard QT_WIDGETS_H */

#ifndef ALGORITHM_H
#define ALGORITHM_H
#include <algorithm>
#endif /* Include guard ALGORITHM_H */

#include "osm_elements.h"
#include "edge.h"
#include "coord_handler.h"

namespace ns_osm {

class Item_Edge : public QGraphicsObject, public Osm_Subscriber, public Edge {
	Q_OBJECT
signals:
	void					signal_edge_clicked	(QPointF,
	                                             Osm_Way*,
	                                             Osm_Node*,
	                                             Osm_Node*,
	                                             Qt::MouseButton);
protected:
//	const Osm_Map&	m_map;
	const Coord_Handler&	m_coord_handler;
	Osm_Way&				m_way;

	void					mouseReleaseEvent	(QGraphicsSceneMouseEvent *event) override;
	void					handle_event_update	(Osm_Node&) override;
public:
	enum {Type = UserType + 2};

	int						type				() const override;
	QRectF					boundingRect		() const override;
	void					paint				(QPainter *painter,
	                                             const QStyleOptionGraphicsItem *option,
	                                             QWidget *widget) override;
	                        Item_Edge			(const Coord_Handler&,
							                     Osm_Node& node1,
							                     Osm_Node& node2,
							                     Osm_Way&,
							                     QGraphicsItem* p_parent = nullptr);
							Item_Edge			(const Coord_Handler&,
							                     const Edge&,
							                     Osm_Way&,
							                     QGraphicsItem* p_parent = nullptr);
							Item_Edge			() = delete;
							Item_Edge			(const Item_Edge&) = delete;
	Item_Edge&				operator=			(const Item_Edge&) = delete;
	virtual					~Item_Edge			();
};

}
#endif // ITEM_EDGE_H
