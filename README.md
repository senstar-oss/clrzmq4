
[![Build Status](https://travis-ci.org/zeromq/clrzmq4.svg?branch=master)](https://travis-ci.org/zeromq/clrzmq4) [![Build status](https://ci.appveyor.com/api/projects/status/102sf149379bgwv7/branch/master?svg=true)](https://ci.appveyor.com/project/zeromq/clrzmq4/branch/master)

**ZeroMQ C# library**

clrzmq4 is a .NET wrapper around the native [ZeroMQ/libzmq](https://github.com/zeromq/libzmq) library, and
provides the ZeroMQ namespace.

It is written in C# 5.0, and can be opened and built using Visual Studio 2012+ and MonoDevelop 5+.

It is built as AnyCPU to run on .NET Framework 4.0+ and mono 5+, and can currently load native shared libraries for
i386 and amd64 on Windows, GNU/Linux and Mac OS X.

The pre-built [nuget.org/packages/ZeroMQ/](https://www.nuget.org/packages/ZeroMQ/) includes native shared libraries for Windows (libzmq 4.1.n) and GNU/Linux (libzmq 4.1.n), for Windows i386 and linux/MacOSX x86_64 architectures.

You can place any native shared library or dynamic link libraries into a folder ~/Downloads/clrzmq4/amd64 or ~/Downloads/clrzmq4/i386, any libzmq.dll/.so/.dylib 3.x 4.x will work, important is just that the file name is `libzmq.dll`.

```
    sudo apt-get install libzmq3-dev
```

Get it
- by [downloading the Release](https://github.com/zeromq/clrzmq4/releases)
- using `git clone https://github.com/zeromq/clrzmq4`
- using [nuget](https://www.nuget.org/packages/ZeroMQ/) `PM> Install-Package ZeroMQ` or by [downloading the nupkg](https://www.nuget.org/api/v2/package/ZeroMQ/)

Open it
- using MonoDevelop or Visual Studio
- using Texteditor gedit or GNU emacs

Run it
- You can't run ZeroMQ/clrzmq4 directly, because this is a library.
- using mono framework:
```
msbuild clrzmq4.mono.sln /p:Configuration=Release
mono bin/Release/ZGuideExamples.exe Espresso
```
- using dotnet framwork
```
nuget restore clrzmq4.netstandard.sln
dotnet msbuild clrzmq4.netstandard.sln
dotnet run --project ZGuideExamples/ZGuideExamples.netstandard.csproj Espresso
```

Read [ZeroMQ - The Guide](http://zguide.zeromq.org/cs:all)
- ZeroMQ - The Guide [Examples in C#](http://github.com/metadings/zguide/tree/master/examples/C%23)

Ask questions on [stackoverflow](http://stackoverflow.com/questions/tagged/c%23+zeromq) using tags `C#` `ZeroMQ` !

#### **[HWClient](https://github.com/metadings/zguide/blob/master/examples/C%23/hwclient.cs) Example**
```csharp
public static void HWClient(string[] args)
{
	//
	// Hello World client
	//
	// Author: metadings
	//

	// Create
	// using (var context = new ZContext())
	using (var requester = new ZSocket(ZSocketType.REQ))
	{
		// Connect
		requester.Connect("tcp://127.0.0.1:5555");

		for (int n = 0; n < 10; ++n)
		{
			string requestText = "Hello";
			Console.Write("Sending {0}...", requestText);

			// Send
			requester.Send(new ZFrame(requestText));

			// Receive
			using (ZFrame reply = requester.ReceiveFrame())
			{
				Console.WriteLine(" Received: {0} {1}!", requestText, reply.ReadString());
			}
		}
	}
}
```

#### **[HWServer](https://github.com/metadings/zguide/blob/master/examples/C%23/hwserver.cs)** Example
```csharp
public static void HWServer(string[] args)
{
	//
	// Hello World server
	//
	// Author: metadings
	//

	if (args == null || args.Length < 1)
	{
		Console.WriteLine();
		Console.WriteLine("Usage: ./{0} HWServer [Name]", AppDomain.CurrentDomain.FriendlyName);
		Console.WriteLine();
		Console.WriteLine("    Name   Your name. Default: World");
		Console.WriteLine();
		args = new string[] { "World" };
	}

	string name = args[0];

	// Create
	// using (var context = new ZContext())
	using (var responder = new ZSocket(ZSocketType.REP))
	{
		// Bind
		responder.Bind("tcp://*:5555");

		while (true)
		{
			// Receive
			using (ZFrame request = responder.ReceiveFrame())
			{
				Console.WriteLine("Received {0}", request.ReadString());

				// Do some work
				Thread.Sleep(1);

				// Send
				responder.Send(new ZFrame(name));
			}
		}
	}
}
```

#### **[WUClient](https://github.com/metadings/zguide/blob/master/examples/C%23/wuclient.cs)** Example
```csharp
public static void WUClient(string[] args)
{
	//
	// Weather update client
	// Connects SUB socket to tcp://localhost:5556
	// Collects weather updates and finds avg temp in zipcode
	//
	// Author: metadings
	//

	if (args == null || args.Length < 2)
	{
		Console.WriteLine();
		Console.WriteLine("Usage: ./{0} WUClient [ZipCode] [Endpoint]", AppDomain.CurrentDomain.FriendlyName);
		Console.WriteLine();
		Console.WriteLine("    ZipCode   The zip code to subscribe. Default is 72622 Nürtingen");
		Console.WriteLine("    Endpoint  Where WUClient should connect to.");
		Console.WriteLine("              Default is tcp://127.0.0.1:5556");
		Console.WriteLine();
		if (args.Length < 1)
			args = new string[] { "72622", "tcp://127.0.0.1:5556" };
		else
			args = new string[] { args[0], "tcp://127.0.0.1:5556" };
	}

	// Socket to talk to server
	// using (var context = new ZContext())
	using (var subscriber = new ZSocket(ZSocketType.SUB))
	{
		string connect_to = args[1];
		Console.WriteLine("I: Connecting to {0}...", connect_to);
		subscriber.Connect(connect_to);

		// Subscribe to zipcode
		string zipCode = args[0];
		Console.WriteLine("I: Subscribing to zip code {0}...", zipCode);
		subscriber.Subscribe(zipCode);

		// Process 10 updates
		int i = 0;
		long total_temperature = 0;
		for (; i < 20; ++i)
		{
			using (var replyFrame = subscriber.ReceiveFrame())
			{
				string reply = replyFrame.ReadString();

				Console.WriteLine(reply);
				total_temperature += Convert.ToInt64(reply.Split(' ')[1]);
			}
		}
		Console.WriteLine("Average temperature for zipcode '{0}' was {1}°", zipCode, (total_temperature / i));
	}
}
```

#### **[WUServer](https://github.com/metadings/zguide/blob/master/examples/C%23/wuserver.cs)** Example
```csharp
public static void WUServer(string[] args)
{
	//
	// Weather update server
	// Binds PUB socket to tcp://*:5556
	// Publishes random weather updates
	//
	// Author: metadings
	//

	// Prepare our (context and) publisher
	// using (var context = new ZContext())
	using (var publisher = new ZSocket(ZSocketType.PUB))
	{
		string address = "tcp://*:5556";
		Console.WriteLine("I: Publisher.Bind'ing on {0}", address);
		publisher.Bind(address);

		// Initialize random number generator
		var rnd = new Random();

		while (true)
		{
			// Get values that will fool the boss
			int zipcode = rnd.Next(99999);
			int temperature = rnd.Next(-55, +45);

			// Send message to all subscribers
			var update = string.Format("{0:D5} {1}", zipcode, temperature);
			using (var updateFrame = new ZFrame(update))
			{
				publisher.Send(updateFrame);
			}
		}
	}
}
```

Also look into the [WUProxy](https://github.com/metadings/zguide/blob/master/examples/C%23/wuproxy.cs) Example.

Learn more: ZeroMQ - [The Guide](http://zguide.zeromq.org/cs:all) and the [Examples in C#](http://github.com/metadings/zguide/tree/master/examples/C%23)

