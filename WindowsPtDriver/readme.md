<h2>Windows Intel Processor Trace Driver</h2>
<p>This driver allow to use the Intel PT (Processor Tracing) feature of the latest Intel SkyLake CPUs from Windows.</p>
<p>Version: 0.4</p>
<p>
  News:
  <ul>
    <li>Re-designed the entire driver code. Now it is much more modular and scalable</li>
    <li>Added and tested the support for Kernel-mode tracing</li>
    <li>Added the support for Kernel tracing from the user-mode application (requires Administratives privilege)</li>
    <li>Written some example code for doing Kernel-tracing of the loading/unloading code of a target driver</li>
  </ul>
</p>
<p>
  Unfortunately the driver code is still too young to be compiled as signed.
  You need to enable the Test Signing mode in your Windows OS to be able to use the driver.
  By the way our tests highlighted that the code was very stable.
  Write me a mail (<a href="mailto:info@andrea-allievi.com">info@andrea-allievi.com</a>) if you find some bugs or for any other queries.<br>
  Hoping to find the time, I will be happy to answer.
</p>

<h5>Compiling</h5>
<p>
  Use Visual Studio 2013/2015 to open the Solution file included in this repository and build it<br>
    You have to download and install the latest Windows Driver Kit to proper compile the Driver.<br>
      Windows Driver Kit: <a href="https://developer.microsoft.com/en-us/windows/hardware/windows-driver-kit">developer.microsoft.com/en-us/windows/hardware/windows-driver-kit</a><br>
</p>
<p>Last Revision: 25th November 2016</p>
     