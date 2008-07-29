package etch.bindings.java.msg;

/**
 * Interface used to implement ValueFactory based import / export code
 * for service defined types.
 */
public interface ImportExportHelper
{
	/**
	 * Exports a value by creating an equivalent StructValue.
	 * @param value a service defined type to export.
	 * @return a StructValue representing the exported value.
	 */
	public StructValue exportValue( Object value );

	/**
	 * Imports a value from an equivalent StructValue.
	 * @param struct a StructValue representing the exported value.
	 * @return a service defined type.
	 */
	public Object importValue( StructValue struct );
}
