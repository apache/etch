package etch.compiler.ast;

/**
 * An implementation of tree walker that does nothing. Useful for
 * instances where you want to look at a few things during a tree
 * walk but don't want to look at everything.
 */
public class AbstractTreeWalker implements TreeWalker
{
	public void doExtern( Extern extern )
	{
		// ignore.
	}

	public void doItem( Item item )
	{
		// ignore.
	}

	public void doMixin( Mixin mixin )
	{
		// ignore.
	}

	public void doThrown( Thrown thrown )
	{
		// ignore.
	}

	public void doTypeRef( TypeRef ref )
	{
		// ignore.
	}

	public void postConstant( Constant constant )
	{
		// ignore.
	}

	public void postEnum( Enumx enumx )
	{
		// ignore.
	}

	public void postExcept( Except except )
	{
		// ignore.
	}

	public void postMessage( Message message )
	{
		// ignore.
	}

	public void postModule( Module module )
	{
		// ignore.
	}

	public void postParameter( Parameter parameter )
	{
		// ignore.
	}

	public void postService( Service service )
	{
		// ignore.
	}

	public void postStruct( Struct struct )
	{
		// ignore.
	}

	public void preConstant( Constant constant )
	{
		// ignore.
	}

	public void preEnum( Enumx enumx )
	{
		// ignore.
	}

	public void preExcept( Except except )
	{
		// ignore.
	}

	public void preMessage( Message message )
	{
		// ignore.
	}

	public void preModule( Module module )
	{
		// ignore.
	}

	public void preParameter( Parameter parameter )
	{
		// ignore.
	}

	public void preService( Service service )
	{
		// ignore.
	}

	public void preStruct( Struct struct )
	{
		// ignore.
	}
}
