// This file automatically generated by:
//   Etch Version 0.95/java backend 0.95
//   Wed Aug 15 18:39:49 CDT 2007

package etch.examples.ccmuser;


/**
 */
public interface CcmUser
{
	/**
	 */
	public class UserConfiguration
	{
		/**
		 * Constructs the UserConfiguration. Don't init anything.
		 */
		public UserConfiguration()
		{
			// don't init anything.
		}
		
		/**
		 * Constructs the UserConfiguration.
		 */
		public UserConfiguration
		(
			etch.examples.ccmuser.CcmUser.Device[] devices
		)
		{
			this.devices = devices;
		}

		@Override
		public String toString()
		{
			return String.format( "UserConfiguration(devices=%s)", devices );
		}

		/**
		 */
		public etch.examples.ccmuser.CcmUser.Device[] devices;
	}

	/**
	 */
	public class Device
	{
		/**
		 * Constructs the Device. Don't init anything.
		 */
		public Device()
		{
			// don't init anything.
		}
		
		/**
		 * Constructs the Device.
		 */
		public Device
		(
			String name
			, String description
			, String type
			, etch.examples.ccmuser.CcmUser.Speeddial[] speeddials
		)
		{
			this.name = name;
			this.description = description;
			this.type = type;
			this.speeddials = speeddials;
		}

		@Override
		public String toString()
		{
			return String.format( "Device(name=%s; description=%s; type=%s; speeddials=%s)", name, description, type, speeddials );
		}

		/**
		 */
		public String name;
		/**
		 */
		public String description;
		/**
		 */
		public String type;
		/**
		 */
		public etch.examples.ccmuser.CcmUser.Speeddial[] speeddials;
	}

	/**
	 */
	public class Speeddial
	{
		/**
		 * Constructs the Speeddial. Don't init anything.
		 */
		public Speeddial()
		{
			// don't init anything.
		}
		
		/**
		 * Constructs the Speeddial.
		 */
		public Speeddial
		(
			String pkid
			, String number
			, String label
		)
		{
			this.pkid = pkid;
			this.number = number;
			this.label = label;
		}

		@Override
		public String toString()
		{
			return String.format( "Speeddial(pkid=%s; number=%s; label=%s)", pkid, number, label );
		}

		/**
		 */
		public String pkid;
		/**
		 */
		public String number;
		/**
		 */
		public String label;
	}

}
