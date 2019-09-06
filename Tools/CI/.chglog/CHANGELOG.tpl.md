{{ $repourl := .Info.RepositoryURL -}}
# CHANGELOG
{{ range .Versions }}
## {{ if .Tag.Previous }}[{{ .Tag.Name }}]{{ else }}{{ .Tag.Name }}{{ end }} - {{ datetime "2006-01-02" .Tag.Date }}
{{ range .Commits -}}
- {{ .Header }} ({{ .Author.Name }})
{{- if .Refs }}
{{- range .Refs }}
{{- if .Action }}
    **{{ .Action }} [#{{ .Ref }}]({{ $repourl }}/issues/{{ .Ref }})**
{{- end }}
{{- end }}
{{- end }}
{{ end }}
{{ end -}}
