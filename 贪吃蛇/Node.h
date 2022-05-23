template <class T>
struct Node{
	Node *next,*last;
	T data;
	Node(const T &d);
	Node(const Node &);
	Node *add_last(Node *);
	Node *add_next(Node *);
	void clear();
};
template <class T>
Node<T> *remove(Node<T> *p);
template <class T>
Node<T>::Node(const T &d):next(nullptr),last(nullptr),data(d){}
template <class T>
Node<T>::Node(const Node<T> &p):next(nullptr),last(nullptr),data(p.data){}
template <class T>
Node<T> *Node<T>::add_last(Node<T> *p){
	p->next=this;
	this->last=p;
	return p;
}
template <class T>
Node<T> *Node<T>::add_next(Node<T> *p){
	this->next=p;
	p->last=this;
	return p;
}
template <class T>
void Node<T>::clear(){
	if(next){
		next->clear();
		delete next;
		next=nullptr;
	}
}
template <class T>
Node<T> *remove(Node<T> *p){
	if(p->last){
		p->last->next=p->next;
	}
	if(p->next){
		p->next->last=p->last;
	}
	Node<T> *tmp=p->last;
	delete p;
	return tmp;
}
struct Color{
	float r,g,b;
};
struct Point{
	float x,y;
};
struct Point_AND_Color{
	float x,y;
	float r,g,b;
};