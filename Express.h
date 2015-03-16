#ifndef Express_h
#define Express_h

#include <QtCore/QByteArray>
#include <vector>
#include "DD.h"

#define FIELD_DELIM ','
#define KV_DELIM '='

class Express
{
public:
	class Pair
	{
	public:
		Pair(const QByteArray & buf, bool exact = false);

		void reset();
		bool isValid();
		bool isMatch(const QByteArray & t, const QByteArray & v);
		bool isMatch(const QByteArray & t, const QByteArray & v, int mask, const JcnMatchGrp_Val mg);

		const int matchCount() const { return _matchCount; }		

	private:
		QByteArray	_buf;
		QByteArray	_tag;
		QByteArray 	_val;
		bool		_valistag;
		int			_matchCount;
		bool		_exact;
	};


	Express(const QByteArray & buf, bool exact = false );

	void	andOp()				{ _andOp = true;  } 
	void	orOp(bool p)		{ _andOp = false; }
	void	reset();
	bool	isValid();
	bool	isMatch(const QByteArray & t, const QByteArray & v);
	bool	isMatch(const QByteArray & t, const QByteArray & v, int m, const JcnMatchGrp_Val mg);

	bool	isMatch();		// is and , or  return;
	bool	isAndMatch();	// all pair mus have at least one match
	bool	isOrMatch();	// any [pair must have at least one match
	
	int		size() { return _prs.size(); } 

private:
	QByteArray 			_buf;
	std::vector<Pair>	_prs;
	bool				_andOp;
};

#endif
