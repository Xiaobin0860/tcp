defmodule Tcp.Mixfile do
  use Mix.Project

  def project do
    [
      app: :tcp,
      version: "0.1.0",
      elixir: "~> 1.5",
      start_permanent: Mix.env == :prod,
      deps: deps()
    ]
  end

  # Run "mix help compile.app" to learn about applications.
  def application do
    [
      extra_applications: [:logger]
    ]
  end

  # Run "mix help deps" to learn about dependencies.
  defp deps do
    [
      {:ranch, github: "Xiaobin0860/ranch"},
      {:connection, github: "Xiaobin0860/connection"},
    ]
  end
end
